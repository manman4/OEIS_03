M=32;

a(n) = sum(k=0, n\5, (-1)^k*binomial(n+4, 5*k+4)*binomial(2*k, k)/(k+1));
for(n=0, M, print1(a(n), ", "));