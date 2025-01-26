M=18;

\\ a(n) = 2 * n! * Sum_{k=0..n-1} binomial(2*n+2*k,k)/((2*n+2*k) * (n-k-1)!) for n > 0.
a(n) = if(n==0, 1, 2*n!*sum(k=0, n-1, binomial(2*n+2*k, k)/((2*n+2*k)*(n-k-1)!)));
for(n=0, M, print1(a(n), ", "));