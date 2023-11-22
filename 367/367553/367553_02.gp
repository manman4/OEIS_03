M=13;

a(n) = sum(k=0, n, n^(n-k)*binomial(n, k)*sumdiv(n, d, d^(2*k-n)));
for(n=1, M, print1(a(n),", "))

b(n) = sum(k=0, n, binomial(n, k)*sumdiv(n, d, d^k * (n/d)^(n-k)));
for(n=1, M, print1(a(n)-b(n),", "))

