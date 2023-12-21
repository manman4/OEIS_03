\\ a(n) = 2^n + 3 * Sum_{k=1..n} binomial(n,k) * a(n-k).
a(n) = 2^n + 3 * sum(k=1, n, binomial(n, k) * a(n-k));
for(n=0, 20, print1(a(n), ", "));