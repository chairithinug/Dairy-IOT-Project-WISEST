% This program generates a report in Excel file of difference between the sensor and the
% thermocouple in percentage and absolute value. In addition, it finds the
% minimimun and maximum difference. The difference is found by applying the
% coefficient parameter p specified below.

clear

sensors = 5; % Number of sensors
trials = 3; % Number of trials
p = [1.1078 -4.79753]; % Coefficients
save_file_name = 'Data/Report.xlsx';

report = zeros(sensors*trials,8);
row = 1;
for r = 1:sensors 
    for s = 1:trials 
    clearvars -except r s row report sensors trials p save_file_name
    
    % Read sensor file and thermocouple file
    csv_name = ['Data/Sensor',num2str(r),'_',num2str(s),'.csv'];
    xlsx_name = ['Data/TC',num2str(r),'_',num2str(s),'.xlsx'];
    data_reading = csvread(csv_name);
    data_thermo = xlsread(xlsx_name);

    time_thermo = zeros(length(data_thermo),1);
    data_resampled = zeros(length(time_thermo),1);

    % Find the average of the two couples
    % mean_thermo = data_thermo(:,4); % If provided with average. Otherwise, use below.
    three_couples = [data_thermo(:,1) data_thermo(:,2) data_thermo(:,3)];
    mean_thermo = mean(three_couples,2);

    % Generate a relative time
    for i = 0 : length(time_thermo)-1
        time_thermo(i+1) = i;
    end

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

    % getting indices where y is valid (not NaN)
    idxValid = ~isnan(data_resampled);

    % Fitting
    f = fit(data_resampled(idxValid),mean_thermo(idxValid),'poly1');
    coeff = coeffvalues(f);
    
    % Apply mean of 1-5
    data_cal_2 = p(1)*data_reading(:,8)+p(2);
    data_resampled_cal_2 = p(1)*data_resampled+p(2);

    diff = abs(mean_thermo - data_resampled_cal_2); % Absolute value of difference
    idxValid_diff = ~isnan(diff); % Remove Nan.
    percent_off = diff(idxValid_diff)./mean_thermo(idxValid_diff)*100; 

    % Percent
    POff = mean(percent_off);
    PMax = max(percent_off);
    PMin = min(percent_off);

    % Absolute value
    AOff = mean(diff(idxValid_diff));
    AMax = max(diff(idxValid_diff));
    AMin = min(diff(idxValid_diff));

    report(row,:) = [POff, PMin, PMax, AOff, AMin, AMax, coeff];
    row = row + 1;
    end
end

% Export file
xlswrite(save_file_name,report);