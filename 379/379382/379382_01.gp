\\ a(n) = 2^n * Sum_{k=0..n} binomial(n/2+3*k/2-1/2,k) * binomial(n/2+k/2+1/2,n-k)/(n+k+1).
a(n) = 2^n * sum(k=0, n, binomial(n/2+3*k/2-1/2,k) * binomial(n/2+k/2+1/2,n-k)/(n+k+1));
for(n=0, 21, print1(a(n),", "))

\\ a(n) = 2^n * A106228(n).
for(n=0, 25, print1(a(n)/2^n,", "))
