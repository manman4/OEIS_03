\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*n-7*k,k).
a(n) = sum(k=0, n\4, binomial(2*n-7*k, k));
for(n=0, 42, print1(a(n),", "))