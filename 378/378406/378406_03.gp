\\ a(n) = Sum_{k=0..n} binomial(n,k) * A389060(k).
a389060(n) = sum(k=0, n\2, binomial(n, k)*binomial(3*k, n-2*k));
a(n) = sum(k=0, n, binomial(n,k) * a389060(k));
for(n=0, 25, print1(a(n), ", "))

