\\ a(n) = Sum_{k=0..floor(n/2)} 9^k * binomial((n+k)/3-1,k).
a(n) = sum(k=0, n\2, 9^k * binomial((n+k)/3-1, k));
for(n=0, 29, print1(a(n),", "))
