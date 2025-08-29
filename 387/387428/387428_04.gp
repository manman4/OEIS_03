\\ a(n) = Sum_{k=0..floor(n/2)} 3^(n-2*k) * binomial(2*(n-k),n-k) * binomial(n-k,k).
a(n) = sum(k=0, n\2, 3^(n-2*k) * binomial(2*(n-k),n-k) * binomial(n-k,k));
for(n=0, 23, print1(a(n),", "))
