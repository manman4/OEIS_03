\\ a(n) = Sum_{k=0..floor(n/4)} 2^k * binomial(n+3,4*k+3).
a(n) = sum(k=0, n\4, 2^k * binomial(n+3, 4*k+3));
for(n=0, 31, print1(a(n),", "))   
