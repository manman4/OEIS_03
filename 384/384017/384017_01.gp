M=15;

\\ a(n) = [x^n] Product_{k=0..n} (1 + k*x)^5.
a(n) = polcoef(prod(k=0, n, 1+k*x)^5, n);
for(n=0, M, print1(a(n), ", "));
