M=21;

\\ a(n) = Sum_{k=0..n} binomial(4*k+1,k) * binomial(k+1,n-k)/(4*k+1).
a(n) = sum(k=0, n, binomial(4*k+1, k) * binomial(k+1, n-k) / (4*k+1));
for(n=0, M, print1(a(n), ", "));
 