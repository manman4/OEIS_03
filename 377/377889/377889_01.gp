M=16;

\\ a(n) = n! * Sum_{k=0..n} (3*n-2*k+1)^(k-1) * binomial(4*n-3*k,n-k)/k!.
a(n) = n! * sum(k=0, n, (3*n-2*k+1)^(k-1) * binomial(4*n-3*k,n-k)/k!);
for(n=0, M, print1(a(n),", "));
