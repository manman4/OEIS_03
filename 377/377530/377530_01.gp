M=19;

\\ a(n) = n! * Sum_{k=0..n} k^(n-k) * binomial(k+2,2)/(n-k)!.
a(n) = n! * sum(k=0, n, k^(n-k) * binomial(k+2,2)/(n-k)!);
for(n=0, M, print1(a(n), ", ")) 