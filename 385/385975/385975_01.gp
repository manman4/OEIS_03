M=26;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(2*k,n-k).
a(n) = sum(k=0, n, binomial(n,k)*binomial(2*k,n-k));
for(n=0, M, print1(a(n), ", "))

