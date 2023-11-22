M=13;

a(n) = sum(k=0, n, binomial(n, k)*sigma(n, 2*k));
for(n=1, M, print1(a(n),", "))

