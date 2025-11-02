\\ a(n) = Sum_{k=0..n} binomial(5*n+k+1,n-k).
a(n) = sum(k=0, n, binomial(5*n+k+1, n-k));
for(n=0, 20, print1(a(n),", "));   


