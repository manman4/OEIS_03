M=17;
a002294(n) = binomial(5*n, n)/(4*n + 1);

\\ a(n) = n! * Sum_{k=0..n} k^(n-k) * A002294(k)/(n-k)!.
a(n) = n! * sum(k=0, n, k^(n-k) * a002294(k)/(n-k)!);
for(n=0, M, print1(a(n), ", ")) 