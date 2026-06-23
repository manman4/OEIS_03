\\ a(n) = Sum_{k=0..n} binomial(n+2*k,3*k) * binomial(n+3*k,k-1).
a(n) = sum(k=0, n, binomial(n+2*k,3*k) * binomial(n+3*k,k-1));
for(n=0, 20, print1(a(n), ", "));


