% Clear all objects
clear

% Initialize port which connects between Arduino and laptop
myArduino = arduino('COM3','Uno');

% Set pinMode
configurePin(myArduino,'D13','DigitalOutput');

% Initialize variables
readings = 750; % Number of samples
times = zeros(readings,1);
temperature = zeros(readings,1);
freq = 10; % 10 samples per second

% Equivalent to delay
pause('on')

% Initialize a figure and axes
figure;
hold on;
ylim([0,5]);
title('Temperature vs. Time');
ylabel('Temperature (C)');
xlabel('Time (seconds)');
scatterPlot = scatter(Nan(readings,1),Nan(readings,1),200,'.k');

while true
    writeDigitalPin(myArduino,'D13',0);    
    serial_data = 
    padding
    index = index + 1;
    if index == 30
       % new line
       % serial end
       index = 0;
       writeDigitalPin(myArduino,'D13',1);
       pause(1/2/freq);
       % serial begin
       pause(1/2/freq);
    end
    
    % Reading data and converting them
for i=1:length(serial_data)-1
str_temp = char(string(serial_data(i)));
str_temp(192) = '1';
str_trun =  char(bin2dec(reshape(str_temp(169:192),8,[]).').');
str_flip = fliplr(str_trun);
temp(i) = hex2dec(str_flip)/10;
end

% Record and display in Real-Time
tic;
for r = 1:readings
    times(r) = toc;
    temperature(r) = % serial_data
    scatterPlot.XData(r) = times(r);
    scatterPlot.YData(r) = temperature(r);
end
    
end




