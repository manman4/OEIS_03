\\ a(n) = Sum_{k=0..n} binomial(2*k-2,n-k).
a(n) = sum(k=0, n, binomial(2*k-2, n-k));
for(n=0, 33, print1(a(n),", ")) 

