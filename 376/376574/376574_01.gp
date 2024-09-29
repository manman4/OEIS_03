M=27;

catalan(n) = binomial(2*n,n)/(n+1);

\\ a(n) = Sum_{k=0..floor(n/3)} binomail(n-2*k-1,k) * Catalan(n-3*k).
a(n) = sum(k=0, n\3, binomial(n-2*k-1,k) * catalan(n-3*k));
for(n=0, M, print1(a(n), ", "));