M=18;

a(n) = sum(k=0, n, (-3)^(n-k) * k^k * binomial(n,k));
for(n=0, M, print1(a(n),", "))
