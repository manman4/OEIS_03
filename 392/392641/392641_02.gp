\\ a(n) = Sum_{k=0..n} binomial(2*k,k) * binomial(n+5*k+2,n-k).
a(n) = sum(k=0, n, binomial(2*k, k) * binomial(n+5*k+2, n-k));
for(n=0, 21, print1(a(n), ", "));



