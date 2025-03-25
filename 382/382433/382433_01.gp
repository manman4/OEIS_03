M=18;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * ( binomial(n,k) - binomial(n,k-1) )^5.
a(n) = sum(k=0, n, binomial(n, k) * (binomial(n, k) - binomial(n, k-1))^5);
for(n=0, M, print1(a(n), ", "))

