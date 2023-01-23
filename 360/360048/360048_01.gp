M=34;

a(n) = sum(k=0, n\2, (-1)^k*binomial(n+1, 2*k+1)*binomial(2*k, k)/(k+1));
for(n=0, M, print1(a(n), ", "));