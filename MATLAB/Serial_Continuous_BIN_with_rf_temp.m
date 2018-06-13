%% Setup
clear
delete(instrfind);
s = serial('COM5');
fopen(s);
set(s,'timeout',5);
%% Reading
count = 0;
error_count = 0;
flushinput(s);
tic;
while count<=5000
    data1 = fgetl(s);
    count = count + 1;
    
    % Change the last bit of temperature bytes to 1
    data1(192) = '1';
    
    % Check if the first of packet is 2 and the last of packet is 3
    try
        temp_res = reshape(data1(169:192),8,[]).';
        temp_dec = bin2dec(temp_res).';
        temp_trun = char(temp_dec);
        temp_flip = fliplr(temp_trun);
        temp = hex2dec(temp_flip)/10;
        fprintf("Temp: %.1f\n", temp);
    catch
        error_count = error_count + 1;
        fprintf("Error %d out of %d %s",error_count,count,data1);
    end
end
toc;