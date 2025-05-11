\\ A(n,k) = k^2 * A(n-1,k) + A(n,k-2) for k > 1.
a(n, k) = if(n<0, 0, if(k<2, k^n, k^2 * a(n-1,k) + a(n,k-2)));
for(n=0, 10, for(k=0, n, print1(a(k, n-k), ", ")));
