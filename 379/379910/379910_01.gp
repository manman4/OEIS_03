M=18;

\\ a(n) = -n! * Sum_{k=0..n} (-3*n+3*k-1)^(n-k-1) * binomial(3*n-2*k,k)/(n-k)!.
a(n) = -n! * sum(k=0, n, (-3*n+3*k-1)^(n-k-1) * binomial(3*n-2*k,k)/(n-k)!);
for(n=0, M, print1(a(n),", "));

