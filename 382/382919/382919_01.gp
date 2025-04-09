M=21;

a(n, r=2, s=3, t=3, u=0) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+u*(n-k)+r));
for(n=0, M, print1(a(n), ", "));




