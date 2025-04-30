M=44;

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(n-2*k,k) * binomial(k,n-3*k).
a(n) = sum(k=0, n\3, binomial(n-2*k,k) * binomial(k,n-3*k));
for(n=0, M, print1(a(n),", "));


