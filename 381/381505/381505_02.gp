M=19;

\\ a(n) = n! * Sum_{k=0..n} (-3)^k * (2/3)^(n-k) * binomial(-1/9,k)/(n-k)!.
a(n) = n! * sum(k=0, n, (-3)^k * (2/3)^(n-k) * binomial(-1/9,k)/(n-k)!);
for(n=0, M, print1(a(n),", "))