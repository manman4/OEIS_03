M=21;

a(n) = sum(k=0, n, k!*sum(j=0, k\2, 1/(j!*(k-2*j)!))*abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));