M=23;

\\ a(n) = Sum_{k=0..n} binomial(n,k) * ( binomial(n,k) - binomial(n,k-1) )^2.
a(n) = sum(k=0, n, binomial(n, k) * (binomial(n, k)-binomial(n, k-1))^2);
for(n=0, M, print1((a(n)),", "))     
