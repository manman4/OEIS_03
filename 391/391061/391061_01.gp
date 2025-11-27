\\ a(n) = Sum_{k=0..floor(n/3)} (2*k+1) * binomial(3*n-7*k+1,n-3*k)/(3*n-7*k+1).
a(n) = sum(k=0, n\3, (2*k+1)*binomial(3*n-7*k+1, n-3*k)/(3*n-7*k+1));
for(n=0, 25, print1(a(n), ", "));

;