M=17;

a(n) = sum(k=0, n, 4^k*k!*stirling(n, k, 1));
for(n=0, M, print1(a(n), ", "));