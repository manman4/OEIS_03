M=20;

\\ a(n) = -2 * n! * Sum_{k=0..n} (-n-2)^(n-k-1) * binomial(n+k+1,k)/(n-k)!.
a(n) = -2 * n! * sum(k=0, n, (-n-2)^(n-k-1) * binomial(n+k+1,k)/(n-k)!);    
for(n=0, M, print1(a(n),", "));

