\\ a(n) = Sum_{k=0..floor(n/7)} 49^k * binomial(n/7-1,k).
a(n) = sum(k=0, n\7, 49^k * binomial(n/7-1, k));
for(n=0, 42, print1(a(n),", "))

\\ a(7*n) = 50^(n-1) for n > 0.
for(n=1, 50, print1(a(7*n)-50^(n-1),", "))

for(n=0, 1000, print1((a(n)-1)%7,", "))  
