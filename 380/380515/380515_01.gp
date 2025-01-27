M=17;

\\ a(n) = 3 * n! * Sum_{k=0..n-1} binomial(3*n+k,k)/((3*n+k) * (n-k-1)!) for n > 0.
a(n) = if(n==0, 1, 3*n!*sum(k=0, n-1, binomial(3*n+k, k)/((3*n+k)*(n-k-1)!)));
for(n=0, M, print1(a(n), ", "));