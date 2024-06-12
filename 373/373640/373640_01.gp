\\ a(n) = Sum_{k=0..floor(n/3)} binomial(3*n-8*k,k).
a(n) = sum(k=0, n\3, binomial(3*n-8*k, k));
for(n=0, 37, print1(a(n),", "))