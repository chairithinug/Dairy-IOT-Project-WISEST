%% Setup
clear
delete(instrfind);
s = serial('COM4');
fopen(s);
set(s,'timeout',5);
%% Reading
count = 0;
error_count = 0;
flushinput(s);
tic;
while count<=5000
    data1 = fscanf(s);
    count = count + 1;
    
    % Change the last bit of temperature bytes to 1
    bit = hexToBinaryVector(data1(47),4);
    bit(4) = 1;
    data1(47) = binaryVectorToHex(bit);
    
    % Check if the first of packet is 2 and the last of packet is 3
    if(data1(1) == '2' && data1(length(data1)-2) == '3')
        temp_res = reshape(data1(42:47),2,[]).';
        temp_dec = hex2dec(temp_res);
        temp_trun = char(temp_dec).';
        temp_flip = fliplr(temp_trun);
        temp = hex2dec(temp_flip)/10;
        fprintf("Temp: %.1f\n", temp);
    else
        error_count = error_count + 1;
        fprintf("Error %d out of %d %s",error_count,count,data1);
    end
end
toc;