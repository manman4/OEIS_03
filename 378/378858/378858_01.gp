a(n, r=4, s=1, t=0, u=3) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+u*(n-k)+r));
for(n=0, 25, print1(a(n), ", "))
