M=30;

a(n) = sum(k=0, n\5, (5*k)!*abs(stirling(n, 5*k, 1))/(120^k*k!));
for(n=0, M, print1(a(n), ", "));