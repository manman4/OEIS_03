\\ a(n) = (1/2) * Sum_{k=0..floor(n/3)} (-1)^k * binomial(2*n-4*k+2,2*k+1).
a(n) = sum(k=0, n\3, (-1)^k * binomial(2*n-4*k+2, 2*k+1))/2;
for(n=0, 34, print1(a(n),", ")) 
