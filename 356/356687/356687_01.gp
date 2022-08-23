M=13;

a(n) = n!*sum(k=0, n, k^(2*n)/k!);
for(n=0, M, print1(a(n), ", "));