\\ a(n) = Sum_{k=0..floor(n/2)} 9^k * binomial(2*n/3-k/3-1,k).
a(n) = sum(k=0, n\2, 9^k * binomial(2*n/3-k/3-1,k));
for(n=0, 28, print1(a(n),", "))
for(n=0, 500, print1((a(n)-1)%3,", "))    
