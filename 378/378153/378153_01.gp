M=35;

c(n) = binomial(2*n,n)/(n+1);

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(3*k,n-3*k) * C(k), where C(k) are the Catalan numbers (A000108).
a(n) = sum(k=0, n\3, binomial(3*k,n-3*k) * c(k));
for(n=0, M, print1(a(n),", "))                 

