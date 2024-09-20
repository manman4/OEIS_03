a(n, s=1, t=5) = sum(k=0, n, binomial(t*(n+1), k)*binomial(s*k, n-k))/(n+1);
for(n=0, 21, print1(a(n), ", "))