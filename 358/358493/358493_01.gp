M=22;

a(n) = sum(k=0, n\3, (n-2*k)!/k!);
for(n=0, M, print1(a(n), ", "));