a(n, s=4, t=4) = sum(k=0, n, binomial(t*(n-k+1),k) * binomial(n+(s-1)*k-1,n-k)/(n-k+1) );
for(n=0, 20, print1(a(n), ", "))