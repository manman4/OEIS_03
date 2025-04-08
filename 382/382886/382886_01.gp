M=21;

a(n, r=3, s=2, t=4, u=0) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(s*k, n-k)/(t*k+u*(n-k)+r));
for(n=0, M, print1(a(n), ", "));




