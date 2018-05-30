% This program reads 2 inputs (1 from thermocouple and 1 from the sensor)
% generate a relative time for the thermocouple
% find the moving mean of the sensor data
% then re-adjust the length of the sensor file to be the same as the
% thermocouple file
% so that they can be used to find a linear regression coefficient
% find mean of two thermocouples and use the number to perform a linear
% regression

%% Generate, Re-adjust, Calculate 

% Clear
clear
cla
close all

% Read sensor file and thermocouple file
data_reading = csvread('Sensor1_5K.csv');
data_thermo = xlsread('5-30-2018_sensor1a_t.xlsx');

data_cal = zeros(length(data_reading),1);
time_thermo = zeros(length(data_thermo),1);
data_resampled = zeros(length(time_thermo),1);

% Find the average of the two couples
% mean_thermo = data_thermo(:,4); % If provided with average. Otherwise, use below.
three_couples = [data_thermo(:,1) data_thermo(:,2) data_thermo(:,3)];
mean_thermo = mean(three_couples,2);

%  two_couples = [data_thermo(:,2) data_thermo(:,3)];
%  mean_thermo = mean(two_couples,2);

% Generate a relative time
for i = 0 : length(time_thermo)-1
    time_thermo(i+1) = i;
end

% data_cal = data_reading(:,8)*1.1207-4.8447; % apply JJ's coeff.
% data_cal = data_reading(:,8)*1.075-4.191; % apply sensor#1 coeff.
moving_mean = movmean(data_reading(:,8), 3); % moving average with width of 3. already take care of edges

% Readjust the sensor data
k = 1;
for j = 1:length(time_thermo)
   while (k < length(data_reading) && time_thermo(j) >= round(data_reading(k,7))) % compare generated thermo time to rel_time
       k = k+1;
   end
   data_resampled(j) = moving_mean(k);
   k = k+1;
end

%% Plotting Data
figure('Name','All Plots','NumberTitle','off');

plot(time_thermo, data_thermo(:,1),'LineWidth', 1);
hold on
plot(time_thermo, data_thermo(:,2),'LineWidth', 1);
plot(time_thermo, data_thermo(:,3),'LineWidth', 1);
plot(time_thermo, mean_thermo,'LineWidth', 1);
plot(data_reading(:,7), data_reading(:,8),'LineWidth', 1);
plot(data_reading(:,7), moving_mean,'LineWidth', 1);
plot(time_thermo, data_resampled,'LineWidth', 1);
grid

xlabel('Time (second)')
ylabel('Temperature (C)')
title('Temperature vs Time')
legend('TC_1','TC_2','TC_3','TC_m_e_a_n','Implant','Moving mean','Resampled data')
hold off

%% Linear Regression
figure('Name','Linear Regression','NumberTitle','off');

% getting indices where y is valid (not NaN)
idxValid = ~isnan(data_resampled);

% fitting
f = fit(data_resampled(idxValid),mean_thermo(idxValid),'poly1')
plot(f,data_resampled,mean_thermo)
title('Linear Regression')
xlabel('Reading Data (resampled sensor data)')
ylabel('Expect Data (thermo data)')
grid

%% After applied linear coeff

% Get coefficients
p = coeffvalues(f);
data_cal_2 = p(1)*data_reading(:,8)+p(2);

figure('Name','Applied This Coefficients','NumberTitle','off');
plot(data_reading(:,7), data_reading(:,8),'LineWidth', 2);
hold on
plot(data_reading(:,7), data_cal_2,'LineWidth', 2);
plot(time_thermo, mean_thermo,'LineWidth', 2);
xlabel('Time (second)')
ylabel('Temperature (C)')
title('Temperature vs Time (Between mean and calibrated data)')
legend('Implant','Implant_c_a_l_2','TC_m_e_a_n')
hold off
grid

%% Apply Mean Sensor Coeffs. From 1-5
% p = [1.0916 -4.4078];
p = [1.1092 -4.7602];
data_cal_2 = p(1)*data_reading(:,8)+p(2);
data_resampled_cal_2 = p(1)*data_resampled+p(2);

figure('Name','Applied Known Coefficients','NumberTitle','off');
plot(data_reading(:,7), data_reading(:,8),'LineWidth', 2);
hold on
plot(data_reading(:,7), data_cal_2,'LineWidth', 2);
plot(time_thermo, mean_thermo,'LineWidth', 2);
xlabel('Time (second)')
ylabel('Temperature (C)')
title('Temperature vs Time with Mean Coeff. (Between mean and calibrated data)')
legend('Implant','Implant_c_a_l','TC_m_e_a_n')
hold off
grid

%% Average Error

diff = mean_thermo - data_resampled_cal_2;

idxValid_diff = ~isnan(diff); % Remove Nan.

percent_off = diff(idxValid_diff)./mean_thermo(idxValid_diff)*100; 
percent_off_abs = abs(percent_off); % Absolute different percentage

Off = mean(percent_off_abs);

%% Plotting 3 sets of coeff. This should NOT be used in general!

p1 = [1.093425 -4.24468]; % JJ-1-2-3
data_cal_p1 = p1(1)*data_reading(:,8)+p1(2);

p2 = [1.09594 -4.41274]; % JJ-1-2-3-4
data_cal_p2 = p2(1)*data_reading(:,8)+p2(2);

p3 = [1.10057 -4.7069]; % JJ-1-1
data_cal_p3 = p3(1)*data_reading(:,8)+p3(2);

figure('Name','Applied 3 Sets of Coeff','NumberTitle','off');
plot(data_reading(:,7), data_cal_p1,'LineWidth', 2);
hold on
plot(data_reading(:,7), data_cal_p2,'LineWidth', 2);
plot(data_reading(:,7), data_cal_p3,'LineWidth', 2);
plot(time_thermo, mean_thermo,'LineWidth', 2);
xlabel('Time (second)')
ylabel('Temperature (C)')
title('Temperature vs Time with 3 Different Sets of Coeff')
legend('JJ-1-2-3','JJ-1-2-3-4','JJ-1-4','TC_m_e_a_n')
hold off
grid

%% Find Correlation Coeff. NOT USED.
R = corrcoef(data_resampled_cal_2, mean_thermo,'Rows','complete')

%% Remove Some Points (Manually). This should NOT be used in general!

figure
xlabel('Time (second)')
ylabel('Temperature (C)')
title('Temperature vs Time')

% Truncate
data_reading_trun = data_reading(4494:10000,:); % (3581:10000,:);
data_thermo_trun = data_thermo(1887:4154,:); % (1507:4154,:);

time_thermo_trun = zeros(length(data_thermo_trun),1);
data_resampled_trun = zeros(length(time_thermo_trun),1);

% Generate a relative time
for i = 1886 : length(time_thermo_trun)+1885
    time_thermo_trun(i-1885) = i;
end

moving_mean_trun = movmean(data_reading_trun(:,8), 3); % moving average with width of 3. already take care of edges

% Find the average of the two couples
two_couples_trun = [data_thermo_trun(:,2) data_thermo_trun(:,3)];
mean_thermo_trun = mean(two_couples_trun,2);

% Readjust the sensor data
r = 1;
for s = 1:length(time_thermo_trun)
   while (time_thermo_trun(s) >= round(data_reading_trun(r,7)))
       r = r+1;
   end
   data_resampled_trun(s) = moving_mean_trun(r);
   r = r+1;
end

hold on

plot(time_thermo_trun, data_thermo_trun(:,2),'LineWidth', 1);
plot(time_thermo_trun, data_thermo_trun(:,3),'LineWidth', 1);
plot(time_thermo_trun, mean_thermo_trun,'LineWidth', 1);
plot(data_reading_trun(:,7), data_reading_trun(:,8),'LineWidth', 1);
plot(data_reading_trun(:,7), moving_mean_trun,'LineWidth', 1);
plot(time_thermo_trun, data_resampled_trun,'LineWidth', 1);

xlabel('Time (second)')
ylabel('Temperature (C)')
title('Temperature vs Time (Truncated)')
legend('TC_1','TC_2','TC_m_e_a_n','Implant','Moving mean','Data resampled')
hold off

figure

% getting indices where y is valid (not NaN)
idxValid_trun = ~isnan(data_resampled_trun);

% fitting
f_trun = fit(data_resampled_trun(idxValid_trun),mean_thermo(idxValid_trun),'poly1')
plot(f,data_resampled_trun,mean_thermo_trun)
title('Linear Regression (Truncated)')
xlabel('Reading Data (resampled sensor data)')
ylabel('Expect Data (thermo data)')

% Get coefficients
p_trun = coeffvalues(f_trun);
%data_cal_2_trun = p_trun(1)*data_reading_trun(:,8)+p_trun(2);

 data_cal_2_trun = 1.075*data_reading_trun(:,8)-4.191; % apply
% known coeff.

figure
hold on
plot(data_reading_trun(:,7), data_reading_trun(:,8),'LineWidth', 1);
plot(data_reading_trun(:,7), data_cal_2_trun,'LineWidth', 1);
plot(time_thermo_trun, mean_thermo_trun,'LineWidth', 1);
xlabel('Time (second)')
ylabel('Temperature (C)')
title('Temperature vs Time (Truncated) (Comparision between mean and calibrated data)')
legend('Implant','Implant_c_a_l_2','TC_m_e_a_n')