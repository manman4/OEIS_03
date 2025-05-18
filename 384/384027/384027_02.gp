M=14;

\\ a(n) = [x^(3*n)] Product_{k=0..n-1} (1 + k*x)^4.
a(n) = polcoef(prod(k=0, n-1, (1 + k*x)^4 + x*O(x^(3*n))), 3*n); 
for(n=0, M, print1(a(n),", "));
