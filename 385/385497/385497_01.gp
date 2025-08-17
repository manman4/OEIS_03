\\ a(n) = Sum_{k=0..n} binomial(6*n+1,k).
a(n) = sum(k=0, n, binomial(6*n+1, k));
for(n=0, 19, print1(a(n), ", "));
