\\ a(n) = Sum_{k=0..n} binomial(n,k) * A387358(k).
a387358(n) = sum(k=0, n, binomial(n, k)*binomial(3*k, n-k));
a(n) = sum(k=0, n, binomial(n, k)*a387358(k));
for(n=0, 25, print1(a(n), ", "))

