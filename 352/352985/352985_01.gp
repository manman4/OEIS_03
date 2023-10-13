M=24;

a(n) = sum(k=0, n\2, k^(2*(n-2*k)));
for(n=0, M, print1(a(n), ", "));