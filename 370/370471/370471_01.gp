M=22;

\\ a(n) = Sum{k=0..n} binomial(n,k) * binomial(n/2+5*k/2+1,n)/(n/2+5*k/2+1).
a(n) = sum(k=0, n, binomial(n, k)*binomial(n/2+5*k/2+1, n)/(n/2+5*k/2+1));
for(n=0, M, print1(a(n),", "))  

