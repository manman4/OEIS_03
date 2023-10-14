M=10;

T(n, k) = n!*sum(j=0, n, (-k*(n-j))^j/j!);
for(n=0, M, for(k=0, n, print1(T(k, n-k),", ")));