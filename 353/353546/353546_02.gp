M=19;

a(n) = n!*sum(k=0, n-1, 2^(n-1-k)/((n-k)*k!));
for(n=0, M, print1(a(n), ", "));
