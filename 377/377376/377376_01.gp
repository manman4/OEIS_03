M=23;

\\ a(n) = Sum_{k=1..floor(n/3)} (-1)^(k-1) * (3*k)! * |Stirling1(n,3*k)|/(k * 6^k).
a(n) = sum(k=1, n\3, (-1)^(k-1) * (3*k)! * abs(stirling(n, 3*k, 1))/(k * 6^k));
for(n=0, M, print1(a(n),", "))                 

