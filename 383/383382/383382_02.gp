M=21;

\\ a(n) = n! * Sum_{k=0..n} (-3)^(n-k) * binomial(k+4,4)/(n-k)!.
a(n) = n! * sum(k=0, n, (-3)^(n-k) * binomial(k+4,4)/(n-k)!);
for(n=0, M, print1(a(n),", "))