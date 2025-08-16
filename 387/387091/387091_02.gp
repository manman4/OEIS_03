\\ a(n) = Sum_{k=0..n} binomial(9*n-k,n-k).
a(n) = sum(k=0, n, binomial(9*n-k,n-k));
for(n=0, 17, print1(a(n),", "));
