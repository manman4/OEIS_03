M=28;

\\ a(n) = Sum_{k=0..n} (k+1) * binomial(2*k,2*n-2*k).
a(n) = sum(k=0, n, (k+1)*binomial(2*k, 2*n-2*k));
for(n=0, M, print1((a(n)),", "))     


