M=18;

\\ a(n) = n! * Sum_{k=0..n} 3^(n-k) * binomial(n+2*k/3-1,n-k)/k!.
a(n) = n! * sum(k=0, n, 3^(n-k)*binomial(n+2*k/3-1,n-k)/k!);
for(n=0, M, print1(a(n),", "));






