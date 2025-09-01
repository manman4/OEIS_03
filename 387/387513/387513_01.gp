\\ a(n) = Sum_{k=0..floor(n/3)} 3^(n-2*k) * binomial(n-2*k,k)^2.
a(n) = sum(k=0, n\3, 3^(n-2*k) * binomial(n-2*k,k)^2);
for(n=0, 25, print1(a(n),", "))
