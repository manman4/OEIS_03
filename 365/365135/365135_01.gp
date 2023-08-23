a(n, s=3, t=2) = sum(k=0, n, binomial(t*(n+1),k) * binomial(n+(s-1)*k-1,n-k) )/(n+1);
for(n=0, 21, print1(a(n), ", "))