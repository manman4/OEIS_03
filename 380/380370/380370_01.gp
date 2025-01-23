M=23;

\\ a(n) = Sum_{k=1..floor(n/5)} (-1)^(k-1) * (5*k)! * |Stirling1(n,5*k)|/(k * 120^k).
a(n) = sum(k=1, n\5, (-1)^(k-1) * (5*k)! * abs(stirling(n, 5*k, 1))/(k * 120^k));
for(n=0, M, print1(a(n),", "))                 

