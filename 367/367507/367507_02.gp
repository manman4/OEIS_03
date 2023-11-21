M=18;

a(n) = sum(k=0, n, 2^(n-k) * binomial(n,k) * sigma(n, k));
for(n=1, M, print1(a(n),", "))

