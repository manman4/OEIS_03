\\ a(n) = Sum_{k=0..floor(n/8)} 16^k * binomial(n/8-1,k).
a(n) = sum(k=0, n\8, 16^k * binomial(n/8-1, k));
for(n=0, 48, print1(a(n),", "))

\\ a(8*n) = 17^(n-1) for n > 0.
for(n=1, 50, print1(a(8*n)-17^(n-1),", "))
