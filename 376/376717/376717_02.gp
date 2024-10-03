\\ a(n) = Sum_{k=0..floor(n/3)} binomial(2*n-4*k+1,2*k).
a(n) = sum(k=0, n\3, binomial(2*n-4*k+1, 2*k)); 
for(n=0, 33, print1(a(n),", "))



