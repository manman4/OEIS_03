M=20;

a(n, r=2, s=1, t=4, u=0) = r*sum(k=0, n, binomial(t*k+r*u*(n-k)+r, k)*binomial(s*k, n-k)/(t*k+r*u*(n-k)+r));
for(n=0, M, print1(a(n),", "))  

\\ a(n) = Sum_{k=0..n} binomial(4*k+2,k) * binomial(k,n-k)/(2*k+1).
b(n) = sum(k=0, n, binomial(4*k+2, k) * binomial(k, n-k)/(2*k+1));
for(n=0, 22, print1(a(n)-b(n), ", "))


