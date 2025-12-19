\\ a(n) = Sum_{k=0..floor(n/4)} 2^k * 3^(n-3*k) * binomial(n-3*k+6,6) * binomial(2*(n-3*k),2*k).
a(n) = sum(k=0, floor(n/4), 2^k * 3^(n-3*k) * binomial(n-3*k+6,6) * binomial(2*(n-3*k),2*k));
for(n=0, 21, print1(a(n), ", "));



