\\ a(n) = Sum_{k=0..n} binomial(4*n+k,n-k).
a(n) = sum(k=0, n, binomial(4*n+k, n-k));
for(n=0, 20, print1(a(n),", "));   


