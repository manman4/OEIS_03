\\ a(n) = Sum_{k=0..floor(n/3)} 2^(n-k) * binomial(k,n-3*k)^2.
a(n) = sum(k=0, n\3, 2^(n-k) * binomial(k,n-3*k)^2);
for(n=0, 32, print1(a(n),", "))
