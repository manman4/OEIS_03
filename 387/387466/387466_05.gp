\\ n*A(n,k) = 2*(2*n-1)*A(n-1,k) + 4*k^2*(n-1)*A(n-2,k) for n > 1.
a(n,k) = if(n<2, n+1, (2*(2*n-1)*a(n-1,k) + 4*k^2*(n-1)*a(n-2,k))/n );
for(n=0, 9, for(k=0, n, print1(a(k,n-k),", ")));



