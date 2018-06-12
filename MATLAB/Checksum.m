%% Setup
delete(instrfind);
clear
s = serial('COM5');
fopen(s);
set(s,'timeout',5);
%% ReadData
logicalStr = {'false', 'true'};
while true
    try
        [data,count,msg] = fgetl(s);
    catch
        fprintf("%s\n",msg);
    end
    try
        data_dec = bin2dec(reshape(data(1:240),8,[]).');
        result = 0;
        for i = 2:length(data_dec)-3
            result = bitxor(data_dec(i),result,'uint8');
        end
        fprintf("result: %d checksum: %d %s\n",result,data_dec(28),logicalStr{(result == data_dec(28)) + 1});
    catch
        fprintf("Conversion failed: %s\n",data);
    end   
end