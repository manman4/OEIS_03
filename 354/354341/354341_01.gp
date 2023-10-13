M=22;

a182926(n) = n!*sumdiv(n, d, 1/(d*(n/d)!^d));
a(n) = sum(k=1, n, a182926(k)*binomial(n, k));
for(n=1, M, print1(a(n), ", "));