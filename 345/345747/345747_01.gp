M=19;

a(n) = n!*sum(k=0, n\2, k^(n-2*k)/k!);
for(n=0, M, print1(a(n), ", "));