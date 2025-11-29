\\ a(n) = Sum_{k=0..floor(n/3)} (k+3) * binomial(4*n-11*k+3,n-3*k)/(4*n-11*k+3).
a(n) = sum(k=0, n\3, (k+3)*binomial(4*n-11*k+3, n-3*k)/(4*n-11*k+3));
for(n=0, 21, print1(a(n), ", "));



