\\ a(n) = Sum_{k=0..n} binomial(n+k,2*k) * binomial(n+2*k,k-1).
a(n) = sum(k=0, n, binomial(n+k,2*k) * binomial(n+2*k,k-1));
for(n=0, 21, print1(a(n), ", "));


