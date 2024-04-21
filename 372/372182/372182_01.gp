a(n, r=1, s=1, t=5, u=2) = r*n!*sum(k=0, n, (t*k+u*(n-k)+r)^(k-1)*binomial(s*k, n-k)/k!);
for(n=0, 16, print1(a(n), ", "))

