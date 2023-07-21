a(n) = sum(k=0, n, (-1)^k*binomial(2*k+1, k)*binomial(2*k+1, n-k)/(2*k+1));
for(n=0, 34, print1(a(n),", "))  