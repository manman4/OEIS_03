a(n) = sum(k=0, n, (-1)^k*binomial(n+3*k+1, k)*binomial(n+3*k+1, n-k)/(n+3*k+1));
for(n=0, 27, print1(a(n),", "))  