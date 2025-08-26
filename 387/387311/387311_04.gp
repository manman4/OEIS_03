\\ a(n) = [x^n] (1+7*x+9*x^2)^(n+3).
a(n) = polcoef((1+7*x+9*x^2)^(n+3), n);
for(n=0, 23, print1(a(n),", "))
