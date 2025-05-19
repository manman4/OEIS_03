\\ a(n) = [x^n] Product_{k=0..n-1} (1 + k*x)^n.
a(n) = polcoef(prod(k=0, n-1, 1+k*x)^n, n);
for(n=0, 15, print1(a(n),", "));


