M=18;

\\ a(n) = n! * Sum_{k=0..n-1} (3*k+1)^(n-k-1) * binomial(n+k,k)/((n+k) * (n-k-1)!) for n > 0.
a(n) = if(n==0, 1, n! * sum(k=0, n-1, (3*k+1)^(n-k-1) * binomial(n+k,k)/((n+k) * (n-k-1)!)));
for(n=0, M, print1(a(n),", "));

