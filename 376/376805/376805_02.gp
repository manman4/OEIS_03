\\ a(n) = Sum_{k=0..floor(n/2)} (-9)^(n-2*k) * binomial(-1/3,n-2*k) * binomial(3*n-5*k,k).
a(n) = sum(k=0, n\2, (-9)^(n-2*k) * binomial(-1/3, n-2*k) * binomial(3*n-5*k, k));
for(n=0, 20, print1(a(n),", "))



