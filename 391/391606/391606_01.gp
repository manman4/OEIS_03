\\ a(n) = Sum_{k=0..n} (-1)^k * (k+1) * (k+3) * binomial(4*n-3*k+3,n-k)/(4*n-3*k+3).
a(n) = sum(k=0, n, (-1)^k * (k+1) * (k+3) * binomial(4*n-3*k+3, n-k)/(4*n-3*k+3));
for(n=0, 21, print1(a(n), ", "));



