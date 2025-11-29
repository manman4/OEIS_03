\\ a(n) = Sum_{k=0..floor(n/2)} (k+3) * binomial(4*n-7*k+3,n-2*k)/(4*n-7*k+3).
a(n) = sum(k=0, n\2, (k+3)*binomial(4*n-7*k+3, n-2*k)/(4*n-7*k+3));
for(n=0, 21, print1(a(n), ", "));



