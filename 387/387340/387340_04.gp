\\ a(n) = [x^n] (1+4*x+3*x^2)^(n+3).
a(n) = polcoef((1+4*x+3*x^2)^(n+3), n);
for(n=0, 23, print1(a(n),", "))
