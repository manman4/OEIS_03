\\ a(n) = [x^n] (1+5*x+x^2)^(n+3).
a(n) = polcoef((1+5*x+x^2)^(n+3), n);
for(n=0, 23, print1(a(n),", "))
