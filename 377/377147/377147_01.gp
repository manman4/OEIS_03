\\ a(n) = Sum_{k=0..floor(n/3)} binomial(k+2,2) * binomial(k,n-3*k)^2.
a(n) = sum(k=0, n\3, binomial(k+2, 2) * binomial(k, n-3*k)^2);
for(n=0, 42, print1(a(n), ", "))
