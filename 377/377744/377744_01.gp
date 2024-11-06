M=16;

\\ a(n) = n! * Sum_{k=0..n} (k+1)^(n-k-1) * binomial(5*k+3,k)/(n-k)!.
a(n) = n! * sum(k=0, n, (k+1)^(n-k-1) * binomial(5*k+3, k)/(n-k)!);
for(n=0, M, print1(a(n),", "));

