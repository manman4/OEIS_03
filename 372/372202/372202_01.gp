a(n, r=2, s=2, t=0, u=1) = r*n!*sum(k=0, n, (t*k+u*(n-k)+r)^(k-1)*binomial(s*k, n-k)/k!);
for(n=0, 18, print1(a(n), ", "))

