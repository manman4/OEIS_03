a(n) = sum(k=0, n, (-9)^k * binomial(-1/3,k) * binomial(3*k,n-k));
for(n=0, 20, print1(a(n),", "))
