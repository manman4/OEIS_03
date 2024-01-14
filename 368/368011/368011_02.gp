M=19;

a(n) = sum(k=0, n\5, binomial(n+k,k) * binomial(6*n+4,n-5*k) )/(n+1);
for(n=0, M, print1(a(n), ", "))
