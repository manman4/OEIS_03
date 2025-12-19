\\ a(n) = Sum_{k=0..floor(n/3)} 2^k * 3^(n-3*k) * binomial(k+6,6) * binomial(2*k,2*(n-3*k)).
a(n) = sum(k=0, n\3, 2^k * 3^(n-3*k) * binomial(k+6,6) * binomial(2*k,2*(n-3*k)));
for(n=0, 29, print1(a(n), ", "));



