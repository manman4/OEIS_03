a(n) = sum(k=0, n, 2^k*binomial(n, k)*binomial(n+3*k+1, n)/(n+3*k+1));
for(n=0, 18, print1(a(n),", ")) 