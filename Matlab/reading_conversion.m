%data = csvread('temp_data.csv');
%baud = 9600;
%s = serial('COM3','BaudRate',baud,'DataBits',8);
clear
data = textread('implant_cali_2018-05-14_16-02-32.txt','%s');
a = string();
for i=1:length(data)-1
str_temp = char(string(data(i)));
str_temp(192) = '1';
str_trun =  char(bin2dec(reshape(str_temp(169:192),8,[]).').');
str_trun = fliplr(str_trun);
temp(i) = hex2dec(str_trun)/10;
end

plot(temp);
