M=23;
\\ a(n) = Sum_{k=0..n} (-1)^k * (k+1) * (k+4) * binomial(3*n-2*k+4,n-k)/(3*n-2*k+4).
a(n) = sum(k=0, n, (-1)^k * (k+1) * (k+4) * binomial(3*n-2*k+4, n-k)/(3*n-2*k+4));
for(n=0, M, print1(a(n),", "));

