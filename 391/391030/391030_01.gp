\\ a(n) = Sum_{k=0..floor(n/4)} (k+1) * binomial(2*n-7*k+1,n-4*k)/(2*n-7*k+1).
a(n) = sum(k=0, n\4, (k+1)*binomial(2*n-7*k+1, n-4*k)/(2*n-7*k+1));
for(n=0, 27, print1(a(n), ", "));

