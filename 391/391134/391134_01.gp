\\ a(n) = Sum_{k=0..floor(n/2)} (k+2) * binomial(2*n-3*k+2,n-2*k)/(2*n-3*k+2).
a(n) = sum(k=0, n\2, (k+2)*binomial(2*n-3*k+2, n-2*k)/(2*n-3*k+2));
for(n=0, 28, print1(a(n), ", "));



