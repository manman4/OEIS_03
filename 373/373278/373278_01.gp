\\ a(n) = Sum_{k=0..floor(n/3)} 9^k * binomial(n/3,k).
a(n) = sum(k=0, n\3, 9^k * binomial(n/3, k));
for(n=0, 32, print1(a(n),", "))

\\ a(3*n) = 10^n for n >= 0.
for(n=0, 300, print1(a(3*n)-10^n,", "))

for(n=0, 500, print1((a(n)-1)%3,", "))
