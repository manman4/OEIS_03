a(n) = if(n==0, 1, sum(k=0, n-1, (-1)^k * binomial(n,k) * binomial(4*n-3*k,n-1-k) )/n);
for(n=0, 22, print1(a(n),", ")) 

b(n, r=1, s=-1, t=4, u=1) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(s*k, n-k)/(t*k+u*(n-k)+r));
for(n=0, 100, print1(a(n)-b(n),", "))
