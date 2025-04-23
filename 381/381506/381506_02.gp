M=18;

\\ a(n) = n! * Sum_{k=0..n} (-4)^k * (3/4)^(n-k) * binomial(-1/16,k)/(n-k)!.
a(n) = n! * sum(k=0, n, (-4)^k * (3/4)^(n-k) * binomial(-1/16,k)/(n-k)!);   
for(n=0, M, print1(a(n),", "))