\\ a(n) = Sum_{k=0..n} (-1)^k * (2*k+3) * binomial(k+2,2) * binomial(4*n-2*k+3,n-k)/(4*n-2*k+3).
a(n) = sum(k=0, n, (-1)^k * (2*k+3) * binomial(k+2,2) * binomial(4*n-2*k+3, n-k)/(4*n-2*k+3));
for(n=0, 22, print1(a(n), ", "));



