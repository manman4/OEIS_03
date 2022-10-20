M=26;

a(n) = sum(k=0, n\5, stirling(n-4*k, k, 1));
for(n=0, M, print1(a(n), ", "));