a(n) = sum(k=0, n, (-1)^k * binomial(-n,k) * binomial(3*k,n-k));

for(n=0, 21, print1(a(n),", "))  