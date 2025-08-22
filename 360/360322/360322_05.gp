M=25;

\\ a(n) = (-1)^n * Sum_{k=0..n} binomial(2*k,k)/(1-2*k) * binomial(n-1,n-k).
a(n) = (-1)^n * sum(k=0, n, binomial(2*k,k)/(1-2*k) * binomial(n-1,n-k));
for(n=0, M, print1(a(n), ", "));