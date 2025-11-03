\\ a(n) = Sum_{k=0..n} binomial(3*n-2*k,n-k).
a(n) = sum(k=0, n, binomial(3*n-2*k, n-k));
for(n=0, 23, print1(a(n),", "));   


