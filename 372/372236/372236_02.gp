a(n, r=1, t=0, u=1/2) = r*sum(k=0, n, (t*n+u*k+r)^(n-1)*binomial(n, k));
for(n=0, 20, print1(a(n), ", "))

