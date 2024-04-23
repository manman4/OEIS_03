a(n, r=1, t=1/2, u=1/2) = r*sum(k=0, n, (t*n+u*k+r)^(n-1)*binomial(n, k));
for(n=0, 17, print1(a(n), ", "))

