M=10;

a(n, k) = binomial((k+1)*n+k-1, n)/(n+1);
for(n=0, M, for(k=0, n, print1(a(k, n-k), ", ")));

\\ For k > 0, A(n,k) = r * binomial(n*p+r,n)/(n*p+r), the Fuss-Catalan number with p=k+1 and r=k.
apr(n, p, r) = r*binomial(n*p+r, n)/(n*p+r);
b(n, k) = if(k==0, 0^n, apr(n, k+1, k));
for(n=0, 20, for(k=0, n, print1(a(k, n-k)-b(k, n-k), ", ")));

