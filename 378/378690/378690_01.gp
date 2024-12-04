M=19;

a(n, r=2, s=1, t=7, u=2) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+u*(n-k)+r));
for(n=0, M, print1(a(n), ", "))

