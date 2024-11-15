M=17;

\\ a(n) = n! * Sum_{k=0..n} (2*k+1)^(k-1) * binomial(n+2*k,n-k)/k!.
a(n) = n! * sum(k=0, n, binomial(n+2*k, n-k) * (2*k+1)^(k-1) / k!);
for(n=0, M, print1(a(n),", "))                 

