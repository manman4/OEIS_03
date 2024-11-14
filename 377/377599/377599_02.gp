M=18;

\\ a(n) = n! * Sum_{k=0..n} (k+1)^(k-1) * binomial(n+2*k,n-k)/k!.
a(n) = n! * sum(k=0, n, binomial(n+2*k, n-k) * (k+1)^(k-1) / k!);
for(n=0, M, print1(a(n),", "))                 

