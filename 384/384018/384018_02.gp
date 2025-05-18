M=18;

\\ a(n) = [x^n] Product_{k=0..n-1} (1 + k*x)^3.
a(n) = polcoef(prod(k=0, n-1, (1 + k*x)^3), n); 
for(n=0, M, print1(a(n),", "));
