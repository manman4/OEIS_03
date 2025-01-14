M=18;

\\ a(n) = 3^n * n! * Sum_{k=0..n} (-1)^k * k^(n-k) * binomial(-1/3,k)/(n-k)!.
a(n) = 3^n * n! * sum(k=0, n, (-1)^k * k^(n-k) * binomial(-1/3,k)/(n-k)!);
for(n=0, M, print1(a(n),", "));

\\ a(n) == 1 (mod 9).
for(n=0, 200, print1(a(n) % 9, ", "));

