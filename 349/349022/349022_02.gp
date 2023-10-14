M=20;

a(n, s=3, t=4) = sum(k=0, n, binomial(t*n-(t-1)*(k-1), k)*binomial(n+(s-1)*k-1, n-k)/(n-k+1));
for(n=0, M, print1(a(n), ", "));