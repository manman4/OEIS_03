\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*k+1,2*n-8*k+1).
a(n) = sum(k=0, n\4, binomial(2*k+1, 2*n-8*k+1));
for(n=0, 49, print1(a(n),", "))



