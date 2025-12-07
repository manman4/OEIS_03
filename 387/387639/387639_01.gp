\\ a(n) = Sum_{k=0..floor(n/2)} (-1)^k * (k+1) * binomial(4*n-7*k+1,n-2*k)/(4*n-7*k+1).
a(n) = sum(k=0, n\2, (-1)^k * (k+1) * binomial(4*n-7*k+1, n-2*k)/(4*n-7*k+1) );
for(n=0, 22, print1(a(n), ", "));
