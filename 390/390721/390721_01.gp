M=21;
\\ a(n) = Sum_{k=0..n} (k+2) * binomial(5*n-4*k+2,n-k)/(5*n-4*k+2).
a(n) = sum(k=0, n, (k+2) * binomial(5*n-4*k+2, n-k)/(5*n-4*k+2));
for(n=0, M, print1(a(n),", "))  