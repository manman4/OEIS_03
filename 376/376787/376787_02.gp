\\ a(n) = Sum_{k=0..floor(n/2)} binomial(2*k+1,2*n-4*k).
a(n) = sum(k=0, n\2, binomial(2*k+1, 2*n-4*k));
for(n=0, 37, print1(a(n),", "))



