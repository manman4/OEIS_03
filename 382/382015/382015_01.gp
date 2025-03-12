M=18;

\\ a(n) = n! * Sum_{k=0..n-1} (k+1)^(n-k-1) * binomial(n+2*k,k)/((n+2*k) * (n-k-1)!) for n > 0.
a(n) = if(n==0, 1, n! * sum(k=0, n-1, (k+1)^(n-k-1) * binomial(n+2*k, k) / ((n+2*k) * (n-k-1)!)));
for(n=0, M, print1(a(n), ", "));


