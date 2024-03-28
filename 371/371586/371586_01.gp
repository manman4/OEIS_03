M=20;

a(n, r=2, s=2, t=4, u=2) = r*sum(k=0, n, binomial(t*k+r*u*(n-k)+r, k)*binomial(s*k, n-k)/(t*k+r*u*(n-k)+r));
for(n=0, M, print1(a(n),", "))  

