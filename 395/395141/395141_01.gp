\\ A(0,k) = 1 and A(n,k) = k*(k+3) * A(n-1,k+6) - k*(k+2) * A(n-1,k+4) for n > 0.
a(n, k) = if(n==0, 1, k*(k+3)*a(n-1, k+6)-k*(k+2)*a(n-1, k+4));
\\ matrix(7, 7, n, k, a(n-1, k-1))

for(n=0, 9, for(k=0, n, print1(a(k, n-k),", ")));
