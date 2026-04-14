

a(n, k) = if(n==0, 1, k*(k+3)*a(n-1, k+6)-k*(k+2)*a(n-1, k+4));
matrix(7, 7, n, k, a(n-1, k-1))