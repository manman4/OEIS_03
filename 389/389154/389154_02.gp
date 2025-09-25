M=29;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389152(k).
a389152(n) = sum(k=0, n\3, binomial(n, k)*binomial(4*k, n-3*k));
a(n) = sum(k=0, n, binomial(n,k)*a389152(k));
for(n=0, M, print1(a(n), ", "))

