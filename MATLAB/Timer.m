tic;
c = 0;
while true
    a = floor(toc / 5);
    if  a > c
        c = a;
        fprintf("Hello %d\n", toc);
    else
        if c > 10
            break
        end
    end
end
fprintf("End %d\n", toc);