\\ Square array A(n,k), n >= 1, k >= 0, read by antidiagonals downwards, where A(n,k) = lcm(n, n+1, ..., n+k).
a(n, k) = lcm(vector(k+1, j, n+j-1));
for(n=1, 11, for(k=1, n, print1(a(k,n-k),", ")));

