M=29;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389126(k).
a389126(n) = sum(k=0, n\4, binomial(n, k)*binomial(2*k, n-4*k));
a(n) = sum(k=0, n, binomial(n,k)*a389126(k));
for(n=0, M, print1(a(n), ", "))

