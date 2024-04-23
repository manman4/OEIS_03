a(n, r=2, t=0, u=1/2) = r*sum(k=0, n, (t*n+u*k+r)^(n-1)*binomial(n, k));
for(n=0, 19, print1(a(n), ", "))

