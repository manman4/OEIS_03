M=18;

\\ a(n) = n! * Sum_{k=0..n} (2*k+1)^(k-1) * binomial(k+1,n-k)/k!.
a(n) = n! * sum(k=0, n, (2*k+1)^(k-1) * binomial(k+1,n-k)/k!);
for(n=0, M, print1(a(n),", "))                 

