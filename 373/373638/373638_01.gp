\\ a(n) = Sum_{k=0..floor(n/2)} binomial(3*n-5*k,k).
a(n) = sum(k=0, n\2, binomial(3*n-5*k, k));
for(n=0, 33, print1(a(n),", "))