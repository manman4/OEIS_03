a(n) = sum(k=0, n, (-1)^k * binomial(-3,k) * binomial(n*k,n-k));

for(n=0, 20, print1(a(n),", "))  