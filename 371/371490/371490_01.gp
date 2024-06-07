\\ a(n) = Sum_{k=0..n} 8^k * binomial((n+3*k)/4-1,k).
a(n) = sum(k=0, n, 8^k * binomial((n+3*k)/4-1,k));
for(n=0, 22, print1(a(n),", "))


