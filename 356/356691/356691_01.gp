M=14;

a(n) = n!*sum(k=0, n, k^(2*k)/k!);
for(n=0, M, print1(a(n), ", "));