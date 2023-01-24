M=33;

a(n) = sum(k=0, n, (-1)^k*binomial(n+3*k+3, n-k)*binomial(2*k, k)/(k+1));
for(n=0, M, print1(a(n), ", "));