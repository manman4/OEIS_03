\\ a(n) = (-1)^n * Sum_{k=0..floor(n/7)} (-2)^k * binomial(n+6,7*k+6).
a(n) = (-1)^n * sum(k=0, n\7, (-2)^k * binomial(n+6, 7*k+6));
for(n=0, 30, print1(a(n),", "))   
