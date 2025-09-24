M=29;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389125(k).
a389125(n) = sum(k=0, n\3, binomial(n, k)*binomial(2*k, n-3*k));
a(n) = sum(k=0, n, binomial(n,k)*a389125(k));
for(n=0, M, print1(a(n), ", "))

