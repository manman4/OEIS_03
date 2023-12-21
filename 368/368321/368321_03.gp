\\ a(n) = 4^n + 2 * Sum_{k=1..n} binomial(n,k) * a(n-k).
a(n) = 4^n + 2 * sum(k=1, n, binomial(n, k) * a(n-k));
for(n=0, 20, print1(a(n), ", "));