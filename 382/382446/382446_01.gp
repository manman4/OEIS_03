M=17;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * ( binomial(n,k) - binomial(n,k-1) )^6.
a(n) = sum(k=0, n, binomial(n, k) * (binomial(n, k)-binomial(n, k-1))^6);
for(n=0, M, print1((a(n)),", "))     
