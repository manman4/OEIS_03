M=18;

a(n) = n!*sum(k=0, n\3, k^n/k!);
for(n=0, M, print1(a(n), ", "));
