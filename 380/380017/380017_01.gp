M=18;

\\ a(n) = n! * Sum_{k=0..n} (-3)^k * k^(n-k) * binomial(-1/3,k)/(n-k)!.
a(n) = n! * sum(k=0, n, (-3)^k * k^(n-k) * binomial(-1/3,k) / (n-k)!);
for(n=0, M, print1(a(n),", "));

