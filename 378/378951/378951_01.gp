M=20;

a(n, r=3, s=-1, t=5, u=3) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(s*k, n-k)/(t*k+u*(n-k)+r));
for(n=0, M, print1(a(n),", "))

