b(n) = if(n==0, 1, sum(k=0, n-1, binomial(n,k) * binomial(5*n-4*k,n-1-k) )/n);
for(n=0, 20, print1(b(n),", ")) 

a(n, r=1, s=1, t=5, u=1) = r*sum(k=0, n, binomial(t*k+u*(n-k)+r, k)*binomial(n+(s-1)*k-1, n-k)/(t*k+u*(n-k)+r));
for(n=0, 100, print1(a(n)-b(n),", "))
