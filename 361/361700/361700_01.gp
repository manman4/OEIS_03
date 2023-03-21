M=29;

a(n) = sum(k=0, n\6, binomial(2*k, k)*binomial(6*k, 2*k)*binomial(n, 6*k));
for(n=0, M, print1(a(n), ", "));
