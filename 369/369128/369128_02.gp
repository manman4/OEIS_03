M=20;

a(n) = sum(k=0, n\5, binomial(n+1,k) * binomial(5*n-5*k+5,n-5*k) )/(n+1);
for(n=0, M, print1(a(n), ", "))
