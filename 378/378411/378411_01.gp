M=29;

a(n, r=2, s=1, t=0, u=3) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(s*k, n-k)/(t*k+u*(n-k)+r));
for(n=0, M, print1(a(n),", "))                 

\\ a(n) = 2 * Sum_{k=0..n} binomial(3*(n-k)+2,k) * binomial(k,n-k)/(3*(n-k)+2).
b(n) = 2 * sum(k=0, n, binomial(3*(n-k)+2,k)*binomial(k,n-k)/(3*(n-k)+2));
for(n=0, 100, print1(a(n)-b(n),", "))


