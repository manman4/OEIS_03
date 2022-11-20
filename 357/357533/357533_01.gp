M=42;

a(n) = sum(k=0, n\4, (n-3*k)!/(n-4*k)!);
for(n=0, M, print1(a(n), ", "));