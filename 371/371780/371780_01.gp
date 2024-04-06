\\ a(n) = Sum_{k=0..floor(n/3)} binomial(5*n+2,n-3*k).
a(n) = sum(k=0, n\3, binomial(5*n+2, n-3*k));
for(n=0, 19, print1(a(n), ", "))

