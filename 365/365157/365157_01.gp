a(n, s=3, t=2) = sum(k=0, n, binomial(t*(n+k+1),k) * binomial(s*k,n-k)/(n+k+1) );
for(n=0, 20, print1(a(n), ", "))