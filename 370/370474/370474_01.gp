M=21;

\\ a(n) = Sum{k=0..n} binomial(n,k) * binomial(3*n/2+3*k/2+1,n)/(3*n/2+3*k/2+1).
a(n) = sum(k=0, n, binomial(n, k)*binomial(3*n/2+3*k/2+1, n)/(3*n/2+3*k/2+1));
for(n=0, M, print1(a(n),", "))  

b(n, r=2, s=-1, t=5, u=3) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(s*k, n-k)/(t*k+u*(n-k)+r));
for(n=0, 100, print1(a(n)-b(n),", "))

