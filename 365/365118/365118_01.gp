a(n, s=1, t=2) = sum(k=0, n, binomial(t*(n-k+1),k) * binomial(n+(s-1)*k-1,n-k)/(n-k+1) );
for(n=0, 26, print1(a(n), ", "))