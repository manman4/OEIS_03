\\ a(n) = Sum_{k=0..floor(n/3)} 9^k * binomial(2*n/3-k-1,k).
a(n) = sum(k=0, n\3, 9^k * binomial(2*n/3-k-1,k));
for(n=0, 33, print1(a(n),", "))
