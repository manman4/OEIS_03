\\ A(0,k) = 1 and A(n,k) = k*(k+4) * A(n-1,k+8) - k*(k+3) * A(n-1,k+6) for n > 0.
a(n, k) = if(n==0, 1, k*(k+4)*a(n-1, k+8)-k*(k+3)*a(n-1, k+6));
\\ matrix(7, 7, n, k, a(n-1, k-1))

for(n=0, 9, for(k=0, n, print1(a(k, n-k),", ")));