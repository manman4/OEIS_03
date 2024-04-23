a(n, r=2, t=1, u=1) = r*sum(k=0, n, (t*n+u*k+r)^(n-1)*binomial(n, k));
for(n=0, 16, print1(a(n), ", "))

