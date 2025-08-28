\\ a(n) = [x^n] (1+5*x+6*x^2)^(n+2).
a(n) = polcoef((1+5*x+6*x^2)^(n+2), n);
for(n=0, 23, print1(a(n),", "))
