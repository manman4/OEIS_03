a(n) = sum(k=0, n, (-9)^k*binomial(-1/3, k)*binomial(n*k, n-k));
for(n=0, 17, print1(a(n),", "))    