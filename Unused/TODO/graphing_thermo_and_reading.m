clear
data_imp = csvread('Readings.csv');
data_thermo = xlsread('thermocouple.xlsx');

figure(1)
plot(data_imp(:,7),data_imp(:,8))
hold on
% plot(ts_the, data_thermo)
plot(data_thermo(1),data_thermo)

legend('Implant','TC1','TC2');
xlabel('Time (s)');
ylabel('Temperature (C)');