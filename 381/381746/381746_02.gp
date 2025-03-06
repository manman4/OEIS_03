M=14;

a118971(n) = binomial(5*n+3,n)/(n+1);
\\ a(n) = Sum_{k=0..2*n} (-1)^k * A118971(k) * A118971(2*n-k).
a(n) = sum(k=0, 2*n, (-1)^k * a118971(k) * a118971(2*n-k));
for(n=0, M, print1(a(n), ", "))
 