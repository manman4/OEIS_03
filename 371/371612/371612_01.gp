M=24;

a(n, r=2, s=1, t=0, u=4) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+u*(n-k)+r));
for(n=0, M, print1(a(n),", "))  

\\ a(n) = Sum_{k=0..n} binomial(4*(n-k)+2,k) * binomial(n-1,n-k)/(2*(n-k)+1).
b(n) = sum(k=0, n, binomial(4*(n-k)+2,k)*binomial(n-1,n-k)/(2*(n-k)+1));
for(n=0, 26, print1(a(n)-b(n), ", "))

