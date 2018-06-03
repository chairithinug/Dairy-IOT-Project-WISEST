h = animatedline;
axis([0 4*pi -1 1])
x = linspace(0,4*pi,2000);
title('Temperature vs. Time');
ylabel('Temperature (C)');
xlabel('Time (seconds)');
for k = 1:length(x)
    y = sin(x(k));
    addpoints(h,x(k),y);
    drawnow
end