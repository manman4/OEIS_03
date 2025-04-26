M=11;

\\ A(0,k) = A(1,k) = 1; A(n,k) = n*A(n-1,k) + k*(n-1)*A(n-2,k).
a(n, k) = if(n<2, 1, n*a(n-1, k) + k*(n-1)*a(n-2, k));
for(n=0, M, for(k=0, n, print1(a(k, n-k),", ")))


