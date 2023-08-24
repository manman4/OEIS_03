a(n, s=2, t=3) = sum(k=0, n, binomial(t*(n+k+1),k) * binomial(s*k,n-k)/(n+k+1) );
for(n=0, 18, print1(a(n), ", "))