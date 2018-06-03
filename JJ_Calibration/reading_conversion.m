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
data_thermo = xlsread('thermocouple.xlsx');

for i=1:length(data)-1 % drop the last incomplete packet
ts_imp(i) = i*3600/(length(data)-1);
end
for i=1:length(data_thermo)
ts_the(i) = i*3600/(length(data_thermo));
end
figure(1)
plot(ts_imp,temp,'LineWidth', 2)
hold on
plot(ts_the, data_thermo,'LineWidth', 2)

legend('Implant', 'TC1', 'TC2', 'TC3');
xlabel('Time (s)');
ylabel('Temperature (C)');

imp_43_ix = find(temp<43,1); % find 1st value below 43 degree
imp_33_ix = find(temp<33,1); % find 1st value below 43 degree
t_43_imp = ts_imp(imp_43_ix);
t_33_imp = ts_imp(imp_33_ix);
[c, ix_43_the] = min(abs(ts_the-t_43_imp));
[c, ix_33_the] = min(abs(ts_the-t_33_imp));
% resample the implant data
temp_imp_rs = resample(temp,length(ts_the),length(ts_imp));

%% temperature data only in the effective range from 33 to 43 according to the implant sensor
temp_imp_eff = temp_imp_rs(ix_43_the:ix_33_the);

avg_tc_temp = mean('data_thermo');
avg_tc_temp_eff = avg_tc_temp(ix_43_the:ix_33_the);
% resample the implant data



temp_imp_eff2 = [ones(size(temp_imp_eff',1),1) temp_imp_eff'];

B = temp_imp_eff2\avg_tc_temp_eff';

temp_imp_cal = B(1) + B(2).*temp_imp_eff2(:,2);

figure(2)
plot(1:length(temp_imp_cal),temp_imp_eff2(:,2),'LineWidth',2)
hold on
plot(1:length(temp_imp_cal),temp_imp_cal,'LineWidth', 2)
hold on
plot(1:length(temp_imp_cal), avg_tc_temp_eff,'LineWidth', 2)

legend('Implant (before cali.)','Implant (after cali.)', 'TC (avg.)');
xlabel('Time (s)');
ylabel('Temperature (C)');