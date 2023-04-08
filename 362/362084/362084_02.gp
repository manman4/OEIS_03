a(n) = sum(k=0, n, (-1)^k * binomial(-2,k) * binomial(n*k,n-k));

for(n=0, 21, print1(a(n),", "))  