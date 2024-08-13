\\ a(n) = Sum_{k=0..n} binomial(3*k-2,n-k).
a(n) = sum(k=0, n, binomial(3*k-2, n-k));
for(n=0, 29, print1(a(n),", ")) 

