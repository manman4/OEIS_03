M=20;

s(r, n) = sum(k=0, n\2, (binomial(n, k)-binomial(n, k-1))^r);
a(n) = s(5, n)/s(1, n);
for(n=0, M, print1(a(n), ", "));



