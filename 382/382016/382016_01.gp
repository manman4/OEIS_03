M=17;

\\ a(n) = n! * Sum_{k=0..n-1} (k+1)^(n-k-1) * binomial(n+3*k,k)/((n+3*k) * (n-k-1)!) for n > 0.
a(n) = if(n==0, 1, n! * sum(k=0, n-1, (k+1)^(n-k-1) * binomial(n+3*k, k) / ((n+3*k) * (n-k-1)!)));  
for(n=0, M, print1(a(n), ", "));


