\\ a(n) = Sum_{k=0..floor(n/5)} 25^k * binomial(n/5,k).
a(n) = sum(k=0, n\5, 25^k * binomial(n/5, k));
for(n=0, 35, print1(a(n),", "))

\\ a(5*n) = 26^n for n >= 0.
for(n=0, 300, print1(a(5*n)-26^n,", "))

for(n=0, 500, print1((a(n)-1)%5,", "))
