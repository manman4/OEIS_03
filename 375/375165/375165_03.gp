\\ a(n) = Sum_{k=0..floor(n/6)} 2^k * binomial(n+5,6*k+5).
a(n) = sum(k=0, n\6, 2^k * binomial(n+5, 6*k+5));
for(n=0, 31, print1(a(n),", "))   
