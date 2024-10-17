\\ a(n) = Sum_{k=0..floor(n/2)} (k+1) * binomial(k,n-2*k)^2.
a(n) = sum(k=0, n\2, (k+1)*binomial(k, n-2*k)^2);
for(n=0, 38, print1(a(n), ", "));

