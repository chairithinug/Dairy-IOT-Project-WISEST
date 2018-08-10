clear;
close All;
delete(instrfindall); 

% Serial comm variables
s = serial('com3'); % need to determine which port is being connected
set(s,'Timeout',2);
fopen(s);

% Data variables
samples = 30; % change ?? to a number of samples
time = zeros(samples,1);
temperature = zeros(samples,1);
readings = zeros(samples,8);

% Plotting variables
h = animatedline;
clearpoints(h);
drawnow
title('Temperature vs. Sample');
ylabel('Temperature (C)');
xlabel('Samples (#)');

% Actual program starts here
for i = 1:samples
    % Get data and time
    data = zeros(240,1);
    [data,count,msg] = fgetl(s);
    time = clock;
    
    if msg == "A timeout occurred before the Terminator was reached."
        temperature = 0;
        warning_inc = 1;
    else
        % Convert BIN to Celsius
        str_temp = data;
        str_temp(192) = '1';
        str_trun =  char(bin2dec(reshape(str_temp(169:192),8,[]).').');
        str_flip = fliplr(str_trun);
        temperature = hex2dec(str_flip)/10;
        warning_inc = 0;
    end
    
    % Animate graph
    addpoints(h,i,temperature);
    drawnow
    
    % Export to file
    readings(i,:) = [time i temperature];
    csvwrite('Readings.csv',readings)
end

% Release variables
fclose(s);
delete(s)
clear s