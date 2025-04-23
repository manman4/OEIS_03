M=19;

\\ a(n) = (-1)^n * n! * Sum_{k=0..n} (1/4)^(n-2*k) * binomial(-1/16,k)/(n-k)!.
a(n) = (-1)^n * n! * sum(k=0, n, (1/4)^(n-2*k) * binomial(-1/16,k)/(n-k)!);
for(n=0, M, print1(a(n),", "))