\\ a(n) = Sum_{k=0..floor(n/3)} binomial(2*n-5*k,k).
a(n) = sum(k=0, n\3, binomial(2*n-5*k, k));
for(n=0, 39, print1(a(n),", "))