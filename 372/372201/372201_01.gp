a(n, r=3, s=3, t=0, u=1) = r*n!*sum(k=0, n, (t*k+u*(n-k)+r)^(k-1)*binomial(n+(s-1)*k-1, n-k)/k!);
for(n=0, 17, print1(a(n), ", "))

