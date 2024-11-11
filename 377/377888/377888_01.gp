M=17;

\\ a(n) = n! * Sum_{k=0..n} (2*n-k+1)^(k-1) * binomial(3*n-2*k,n-k)/k!.
a(n) = n! * sum(k=0, n, (2*n-k+1)^(k-1) * binomial(3*n-2*k,n-k)/k!);
for(n=0, M, print1(a(n),", "));

