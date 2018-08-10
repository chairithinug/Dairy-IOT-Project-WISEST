clear
% Initialize variables
readings = 750; % Number of samples
times = zeros(readings,1);
temperature = zeros(readings,1);

s = serial('com3');
fopen(s);

% Initialize a figure and axes
figure;
hold on;
ylim([0,5]);
title('Temperature vs. Time');
ylabel('Temperature (C)');
xlabel('Time (seconds)');
gplot = scatter(zeros(readings,1),zeros(readings,1),200,'.k');

data = fgetl(s);
data = data(1:240);

% Reading data and converting them
for i=1:length(data)-1
str_temp = char(string(data(i)));
str_temp(192) = '1';
str_trun =  char(bin2dec(reshape(str_temp(169:192),8,[]).').');
str_flip = fliplr(str_trun);
temp(i) = hex2dec(str_flip)/10;
end

% Record and display in Real-Time
tic;
for r = 1:length(data)-1
    times(r) = toc;
    temperature(r) = temp(i);
    gplot.XData(r) = times(r);
    gplot.YData(r) = temperature(r);
end