\\ a(n) = Sum_{k=0..floor(n/3)} binomial(3*n-8*k-1,k).
a(n) = sum(k=0, n\3, binomial(3*n-8*k-1, k));
for(n=0, 37, print1(a(n),", "))