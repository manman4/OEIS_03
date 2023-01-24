M=41;

a(n) = sum(k=0, n, (-1)^k*binomial(n+2*k+2, n-k)*binomial(2*k, k)/(k+1));
for(n=0, M, print1(a(n), ", "));