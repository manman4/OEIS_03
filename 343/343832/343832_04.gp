M=30;

a(n) = (n-1)!*pollaguerre(n-1, n, -1);
for(n=1, M, print1(a(n), ", "));