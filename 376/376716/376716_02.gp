\\ a(n) = Sum_{k=0..floor(n/2)} binomial(2*n-2*k+1,2*k).
a(n) = sum(k=0, n\2, binomial(2*n-2*k+1, 2*k));
for(n=0, 29, print1(a(n),", "))



