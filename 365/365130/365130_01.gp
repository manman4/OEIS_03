a(n, s=2, t=3) = sum(k=0, n, binomial(t*(n+1),k) * binomial(s*k,n-k) )/(n+1);
for(n=0, 20, print1(a(n), ", "))