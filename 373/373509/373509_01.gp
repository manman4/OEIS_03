\\ a(n) = Sum_{k=0..floor(n/4)} 8^k * binomial(n/4-1,k).
a(n) = sum(k=0, n\4, 8^k * binomial(n/4-1, k));
for(n=0, 39, print1(a(n),", "))

\\ a(4*n) = 9^(n-1) for n > 0.
for(n=1, 50, print1(a(4*n)-9^(n-1),", "))
