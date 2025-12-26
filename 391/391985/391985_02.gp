\\ a(n) = Sum_{k=0..floor(n/4)} 2^k * 3^(n-3*k) * binomial(k+2,2) * binomial(n-2*k,k) * binomial(n-3*k,k).
a(n) = sum(k=0, n\4, 2^k * 3^(n-3*k) * binomial(k+2,2) * binomial(n-2*k,k) * binomial(n-3*k,k));
for(n=0, 26, print1(a(n), ", "));



