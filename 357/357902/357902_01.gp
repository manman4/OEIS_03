M=26;

a(n) = sum(k=0, n\4, abs(stirling(n-3*k, k, 1)));
for(n=0, M, print1(a(n), ", "));