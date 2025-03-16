M=10;

\\ A(n,k) = (n-1)! * LaguerreL(n-1, (k-1)*n+1, -1) for n > 0.
a(n, k) = if(n==0, 1, (n-1)!*pollaguerre(n-1, (k-1)*n+1, -1));
for(n=0, M, for(k=0, n, print1(a(k,n-k),", "))) 


