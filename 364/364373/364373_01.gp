a(n) = sum(k=0, n, (-1)^k*binomial(4*k+1, k)*binomial(4*k+1, n-k)/(4*k+1));
for(n=0, 26, print1(a(n),", "))  