

a(n,k)=if(n==0, 1, (k^2 + 3*k) * a(n-1,k+6) - (k^2 + 2*k) * a(n-1,k+4));
matrix(7, 7, n, k, a(n-1, k-1))