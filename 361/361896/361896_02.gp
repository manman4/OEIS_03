a(n) = sum(k=0, n, (-9)^k * binomial(-1/3,k) * binomial(n+3*k-1,n-k));
for(n=0, 20, print1(a(n),", "))
