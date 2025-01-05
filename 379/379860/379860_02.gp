M=17;

\\ a(n) = (n!/(n+1)) * Sum_{k=0..n} (n+k+1)^(n-k) * binomial(2*n+k+1,k)/(n-k)!.
a(n) = n! * sum(k=0, n, (n+k+1)^(n-k) * binomial(2*n+k+1,k)/(n-k)!)/(n+1);
for(n=0, M, print1(a(n),", "));

