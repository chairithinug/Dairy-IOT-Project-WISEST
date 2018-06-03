clear
delete(instrfindall); 

s = serial('com3'); % need to determine which port is being connected
fopen(s);

samples = 30; % change ?? to a number of samples
time = zeros(samples,1);
temperature = zeros(samples,1);
data = zeros(240,samples);

tic;
for i = 1:samples
    data(:,i) = fgetl(s);
    time(i) = toc;
end

for i = 1:samples
    data_trun = data(1:240);
    str_temp = data_trun;
    str_temp(192) = '1';
    str_trun =  char(bin2dec(reshape(str_temp(169:192),8,[]).').');
    str_flip = fliplr(str_trun);
    temperature(i) = hex2dec(str_flip)/10;
end
    
% Plot the graph
plot(time,temperature);
title('Temperature vs. Time');
ylabel('Temperature (C)');
xlabel('Time (seconds)');

fclose(s)
delete(s)
clear s