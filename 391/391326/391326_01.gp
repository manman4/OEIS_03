\\ a(n) = Sum_{k=0..floor(n/3)} (-1)^k * (3*k+1) * binomial(4*n-9*k+1,n-3*k)/(4*n-9*k+1).
a(n) = sum(k=0, n\3, (-1)^k * (3*k+1) * binomial(4*n-9*k+1, n-3*k)/(4*n-9*k+1) );
for(n=0, 22, print1(a(n), ", "));
