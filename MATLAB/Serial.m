%% Setup
clear
delete(instrfind);
s = serial('COM5');
fopen(s);
set(s,'timeout',5);
%% Reading
while true
    data1 = fscanf(s);
    bit = hexToBinaryVector(data1(47),4);
    bit(4) = 1;
    data1(47) = binaryVectorToHex(bit);
    if(data1(1) == '2' && data1(length(data1)-2) == '3')
        temp_res = reshape(data1(42:47),2,[]).';
        temp_dec = hex2dec(temp_res);
        temp_trun = char(temp_dec).';
        temp_flip = fliplr(temp_trun);
        temp = hex2dec(temp_flip)/10;
        fprintf("Temp: %.1f\n", temp);
    else
        fprintf("Error %s",data1);
    end
end