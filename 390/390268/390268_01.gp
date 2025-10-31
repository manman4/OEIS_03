\\ a(n) = Sum_{k=0..n} binomial(2*n+3*k+1,n-k).
a(n) = sum(k=0, n, binomial(2*n+3*k+1, n-k));
for(n=0, 24, print1(a(n),", "));   


