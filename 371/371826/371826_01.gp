\\ a(n) = Sum_{k=0..floor(n/2)} n^k * binomial(2*n-k,n-2*k).
a(n) = sum(k=0, n\2, n^k * binomial(2*n-k, n-2*k));
for(n=0, 22, print1(a(n), ", "))

