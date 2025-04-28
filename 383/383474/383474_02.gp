M=10;

\\ A(n,k) = A(n-1,k) + A(n-2,k) + A(n-3,k) + A(n,k-1) + A(n,k-2) + A(n,k-3).
a(n, k) = if(n<0 || k < 0, 0, if(n==0 && k==0, 1, a(n-1, k) + a(n-2, k) + a(n-3, k) + a(n, k-1) + a(n, k-2) + a(n, k-3)));
for(n=0, M, for(k=0, n, print1(a(k, n-k), ", ")));


