M=30;

a(n) = sum(k=0, n\5, (5*k)!*stirling(n, 5*k, 2)/120^k);
for(n=0, M, print1(a(n), ", "));