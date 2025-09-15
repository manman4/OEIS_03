\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(2*n+k+1,k).
a(n) = sum(k=0, n, binomial(n, k) * binomial(2*n+k+1, k));
for(n=0, 19, print1(a(n),", "));


