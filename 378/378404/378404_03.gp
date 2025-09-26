\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389225(k).
a389225(n) = sum(k=0, n, binomial(n, k)*binomial(4*k, n-k));
a(n) = sum(k=0, n, binomial(n, k)*a389225(k));
for(n=0, 25, print1(a(n), ", "))

