M=18;

\\ a(n) = n! * Sum_{k=0..n} (-4)^k * (k+1)^(n-k) * binomial(-1/4,k)/(n-k)!.
a(n) = n! * sum(k=0, n, (-4)^k * (k+1)^(n-k) * binomial(-1/4,k) / (n-k)!);
for(n=0, M, print1(a(n),", "));

