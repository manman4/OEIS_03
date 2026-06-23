\\ a(n) = Sum_{k=0..n} binomial(n+3*k,4*k) * binomial(n+4*k,k-1).
a(n) = sum(k=0, n, binomial(n+3*k,4*k) * binomial(n+4*k,k-1));
for(n=0, 19, print1(a(n), ", "));


