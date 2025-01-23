M=23;

\\ a(n) = Sum_{k=1..floor(n/4)} (4*k)! * |Stirling1(n,4*k)|/(k * 24^k).
a(n) = sum(k=1, n\4, (4*k)! * abs(stirling(n, 4*k, 1))/(k * 24^k));
for(n=0, M, print1(a(n),", "))                 

