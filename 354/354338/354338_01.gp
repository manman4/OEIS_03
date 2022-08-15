M=22;

a087906(n) = n!*sumdiv(n, d, 1/(d*(n/d)!));
a(n) = sum(k=1, n, a087906(k)*binomial(n, k));
for(n=1, M, print1(a(n), ", "));