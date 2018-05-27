% Set up
clear
% a = arduino('COM3','Uno');
% configurePin(a,'D13','DigitalOutput');
% writeDigitalPin(a,'D13',1);
s = serial('com3');
fopen(s);
index = 1;
serial_data = zeros(30,1);
% serial begin

% Forever
while true
%     writeDigitalPin(a,'D13',1);
    serial_data(index) = fread(s);
    index = index + 1;
	if index == 31
       % serial end
       index = 1;
%        writeDigitalPin(a,'D13',0);
       pause(0.5);
       % serial begin
       pause(0.5);
    end
end    