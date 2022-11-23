M=22;

a(n) = sum(k=0, n\5, (-1)^k*(n-5*k)!);
for(n=0, M, print1(a(n), ", "));