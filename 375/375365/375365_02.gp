\\ a(n) = Sum_{k=0..floor(n/2)} binomial(3*k-2,n-2*k).
a(n) = sum(k=0, n\2, binomial(3*k-2, n-2*k));
for(n=0, 37, print1(a(n),", ")) 

