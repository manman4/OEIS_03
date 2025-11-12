\\ a(n) = Sum_{k=0..floor(n/2)} binomial(3*n-2*k-2,n-2*k).
a(n) = sum(K=0, n\2, binomial(3*n - 2*K - 2, n - 2*K));
for(n=0, 20, print1(a(n),", "));
