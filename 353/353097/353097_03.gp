M=22;

b(n, k) = sum(j=0, n-1, (k-n+j)*k^j);
a(n) = b(n, 6);
for(n=1, M, print1(a(n), ", "));