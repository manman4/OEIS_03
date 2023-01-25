M=30;

a(n) = sum(k=0, n, (-1)^k*binomial(3*k, n-k)*binomial(2*k, k)/(k+1));
for(n=0, M, print1(a(n), ", "));