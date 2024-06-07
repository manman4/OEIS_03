\\ a(n) = Sum_{k=0..floor(n/3)} 8^k * binomial((n+k)/4-1,k).
a(n) = sum(k=0, n\3, 8^k * binomial((n+k)/4-1,k));
for(n=0, 35, print1(a(n),", "))


