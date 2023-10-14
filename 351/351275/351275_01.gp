M=16;

a(n) = sum(k=0, n, (-2*k)^k*stirling(n, k, 1));
for(n=0, M, print1(a(n), ", "));