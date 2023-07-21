a(n) = sum(k=0, n, (-1)^k*binomial(3*k+1, k)*binomial(3*k+1, n-k)/(3*k+1));
for(n=0, 30, print1(a(n),", "))  