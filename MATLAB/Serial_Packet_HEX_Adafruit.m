%% Setup
clear
delete(instrfind);
s = serial('COM6');
s.Baudrate = 115200;
fopen(s);
set(s,'timeout',5);
%% Get data
error_count = 0;
count = 0;
flushinput(s);
tic;
while count<=5000
    count = count + 1;
    time = clock;
    data1 = fscanf(s); % data1(7:64) RSSI = data1(72:76)
    % Change the last bit of temperature bytes to 1
    bit = hexToBinaryVector(data1(53),4);
    bit(4) = 1;
    data1(53) = binaryVectorToHex(bit);
    
    % Check if the first of packet is 2 and the last of packet is 3
    if(data1(7) == '2' && data1(65) == '3')
        temp_res = reshape(data1(48:53),2,[]).';
        temp_dec = hex2dec(temp_res);
        temp_trun = char(temp_dec).';
        temp_flip = fliplr(temp_trun);
        temp = hex2dec(temp_flip)/10;
        fprintf("Temp: %.1f %.3f\n", temp,time(:));
    else
        error_count = error_count + 1;
        fprintf("Error %d out of %d %s",error_count,count,data1);
    end
end
toc;