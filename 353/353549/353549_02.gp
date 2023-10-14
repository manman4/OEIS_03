M=18;

a(n) = n!*sum(k=0, n-1, (-3)^(n-1-k)/((n-k)*k!));
for(n=0, M, print1(a(n), ", "));