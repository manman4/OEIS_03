M=20;

\\ a(n) = 3^n * n! * Sum_{k=0..n} k^(n-k) * binomial(1/3,k)/(n-k)!.
a(n) = 3^n * n! * sum(k=0, n, k^(n-k) * binomial(1/3,k)/(n-k)!);
for(n=0, M, print1(a(n),", "));

\\ a(n) == 1 (mod 3).
for(n=0, 200, print1(a(n) % 3, ", "));

