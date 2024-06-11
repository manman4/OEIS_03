\\ a(n) = Sum_{k=0..floor(n/5)} 25^k * binomial(n/5-1,k).
a(n) = sum(k=0, n\5, 25^k * binomial(n/5-1, k));
for(n=0, 37, print1(a(n),", "))

\\ a(5*n) = 26^(n-1) for n > 0.
for(n=1, 50, print1(a(5*n)-26^(n-1),", "))

for(n=0, 1000, print1((a(n)-1)%5,", "))  
