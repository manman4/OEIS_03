M=29;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389151(k).
a389151(n) = sum(k=0, n\2, binomial(n, k)*binomial(4*k, n-2*k));
a(n) = sum(k=0, n, binomial(n,k)*a389151(k));
for(n=0, M, print1(a(n), ", "))

