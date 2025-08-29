\\ a(n) = [x^n] (1 + 2*n*x + (n^2+1)*x^2)^n.
a(n) = polcoef((1 + 2*n*x + (n^2+1)*x^2)^n, n);
for(n=0, 23, print1(a(n),", "))
