\\ a(n) = -Sum_{k=1..floor(n/2)} (-1)^k * (n-2*k)! * binomial(n,2*k-1).
a(n) = -sum(k=1, n\2, (-1)^k * (n-2*k)! * binomial(n,2*k-1));
for(n=0, 23, print1(a(n),", "))       