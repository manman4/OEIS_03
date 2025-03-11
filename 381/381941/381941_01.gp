M=21;

\\ a(n) = Sum_{k=0..n} binomial(5*k+1,k) * binomial(2*k+2,n-k)/(5*k+1).
a(n) = sum(k=0, n, binomial(5*k+1, k) * binomial(2*k+2, n-k) / (5*k+1));
for(n=0, M, print1(a(n), ", "));


