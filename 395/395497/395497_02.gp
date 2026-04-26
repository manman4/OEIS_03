a219836(n, k) = if(n==0, k==0, (k+1)*a219836(n-1, k)+(n-k)*a219836(n-1, k-1)+(-1)^n*(k==n-1));
for(n=0, 10, for(k=0, n, print1(a219836(n,n-1-k),", ")));

\\ T(n,k) = A219836(n,n-1-k) for n > 0.
T(n, k) = if(n==0, k==0, a219836(n, n-1-k));
for(n=0, 10, for(k=0, n, print1(T(n,k),", ")));
