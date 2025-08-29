\\ a(n) = [x^n] (1+6*x+10*x^2)^n.
a(n) = polcoef((1+6*x+10*x^2)^n, n);
for(n=0, 23, print1(a(n),", "))
