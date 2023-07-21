a(n) = sum(k=0, n, (-1)^k*binomial(n+2*k+1, k)*binomial(n+2*k+1, n-k)/(n+2*k+1));
for(n=0, 30, print1(a(n),", "))  