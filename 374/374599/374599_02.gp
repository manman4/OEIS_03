\\ a(n) = Sum_{k=0..floor(n/4)} 2^k * binomial(n-3*k,k) * binomial(2*(n-3*k),n-3*k).
a(n) = sum(k=0, n\4, 2^k * binomial(n-3*k, k) * binomial(2*(n-3*k), n-3*k));
for(n=0, 25, print1(a(n), ", "))