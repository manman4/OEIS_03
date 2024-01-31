a000587(n) = sum(k=0, n, (-1)^k * stirling(n, k, 2));
\\ T(n,k) = Sum_{j=0..n} k^j * Stirling1(n,j) * A000587(j).
T(n,k) = sum(j=0, n, k^j * stirling(n, j, 1) * a000587(j));
for(n=0, 10, for(k=0, n, print1(T(k,n-k), ", "))); 