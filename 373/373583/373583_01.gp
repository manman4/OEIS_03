\\ a(n) = Sum_{k=0..floor(n/4)} 16^k * binomial(n/4,k).
a(n) = sum(k=0, n\4, 16^k * binomial(n/4, k));
for(n=0, 33, print1(a(n),", "))

\\ a(4*n) = 17^n for n >= 0.
for(n=0, 300, print1(a(4*n)-17^n,", "))

for(n=0, 500, print1((a(n)-1)%4,", "))
