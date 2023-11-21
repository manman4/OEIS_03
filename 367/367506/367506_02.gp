M=17;

\\ a(n) = Sum_{k=0..n} n^(n-k) * binomial(n,k) * sigma_k(n).
a(n) = sum(k=0, n, n^(n-k) * binomial(n,k) * sigma(n, k));
for(n=1, M, print1(a(n),", "))

