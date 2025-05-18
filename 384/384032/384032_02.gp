M=13;

\\ a(n) = [x^(2*n)] Product_{k=0..n} (1 + k*x)^4.
a(n) = polcoef(prod(k=0, n, (1 + k*x)^4 + x*O(x^(2*n))), 2*n); 
for(n=0, M, print1(a(n),", "));
