M=13;

a(n) = sum(k=0, n, n^k * binomial(n,k) * sigma(n, k));
for(n=1, M, print1(a(n),", "))

