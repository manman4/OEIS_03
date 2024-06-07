\\ a(n) = Sum_{k=0..floor(n/2)} 8^k * binomial((n+2*k)/4-1,k).
a(n) = sum(k=0, n\2, 8^k * binomial((n+2*k)/4-1,k));
for(n=0, 30, print1(a(n),", "))


