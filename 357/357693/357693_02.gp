M=22;

a(n) = sum(k=0, n\2, (-2)^k*stirling(n, 2*k, 1));
for(n=0, M, print1(a(n), ", "));