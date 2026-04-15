\\ A(0,k) = 1 and A(n,k) = k*(k+5) * A(n-1,k+10) - k*(k+4) * A(n-1,k+8) for n > 0.
a(n, k) = if(n==0, 1, k*(k+5)*a(n-1, k+10)-k*(k+4)*a(n-1, k+8));
\\ matrix(7, 7, n, k, a(n-1, k-1))

for(n=0, 9, for(k=0, n, print1(a(k, n-k),", ")));