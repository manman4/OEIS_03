\\ a(n) = Sum_{k=0..floor(n/3)} (-1)^k * (k+1) * binomial(3*n-8*k+1,n-3*k)/(3*n-8*k+1).
a(n) = sum(k=0, n\3, (-1)^k * (k+1) * binomial(3*n-8*k+1, n-3*k)/(3*n-8*k+1));
for(n=0, 24, print1(a(n), ", "));



