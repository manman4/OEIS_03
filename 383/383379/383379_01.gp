M=19;

\\ a(n) = n! * Sum_{k=0..n} (-n)^(n-k) * binomial(n+k,n)/(n-k)!.
a(n) = n! * sum(k=0, n, (-n)^(n-k) * binomial(n+k,n)/(n-k)!);
for(n=0, M, print1(a(n),", "))