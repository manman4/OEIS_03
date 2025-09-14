\\ a(n) = Sum_{k=0..n} binomial(n,k) * binomial(n^2+k+1,n).
a(n) = sum(k=0, n, binomial(n,k) * binomial(n^2+k+1,n));
for(n=0, 16, print1(a(n), ", "));


