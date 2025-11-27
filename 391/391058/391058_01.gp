\\ a(n) = Sum_{k=0..floor(n/2)} (k+1) * binomial(3*n-5*k+1,n-2*k)/(3*n-5*k+1).
a(n) = sum(k=0, n\2, (k+1)*binomial(3*n-5*k+1, n-2*k)/(3*n-5*k+1));
for(n=0, 25, print1(a(n), ", "));

;