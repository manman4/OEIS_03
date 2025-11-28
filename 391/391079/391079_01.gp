\\ a(n) = Sum_{k=0..floor(n/3)} (2*k+2) * binomial(4*n-10*k+2,n-3*k)/(4*n-10*k+2).
a(n) = sum(k=0, n\3, (2*k+2)*binomial(4*n-10*k+2, n-3*k)/(4*n-10*k+2));
for(n=0, 21, print1(a(n), ", "));



