\\ a(n) = Sum_{k=0..floor(n/2)} (2*k+1) * binomial(4*n-4*k+2,n-2*k)/(2*n-2*k+1).
a(n) = sum(k=0, n\2, (2*k+1)*binomial(4*n-4*k+2, n-2*k)/(2*n-2*k+1));
for(n=0, 21, print1(a(n), ", "));



