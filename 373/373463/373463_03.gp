\\ a(n) = (-1)^n * Sum_{k=0..floor(n/5)} (-2)^k * binomial(n+4,5*k+4).
a(n) = sum(k=0, n\5, (-2)^k * binomial(n+4, 5*k+4));
for(n=0, 31, print1(a(n),", "))   
