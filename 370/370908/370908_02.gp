M=17;

\\ a(n) = 1/(4*(n+1)) * Sum_{k=0..n+1} 3^(n+1-k) * k^n * binomial(n+1,k).
a(n) = 1/(4*(n+1)) * sum(k=0, n+1, 3^(n+1-k) * k^n * binomial(n+1,k));
for(n=0, M, print1(a(n), ", "))