\\ a(n) = Sum_{k=1..floor(n/4)} (-1)^(k-1) * (4*k)! * Stirling2(n,4*k)/(k * 24^k).
a(n) = sum(k=1, n\4, (-1)^(k-1) * (4*k)! * stirling(n, 4*k, 2)/(k * 24^k));
for(n=4, 25, print1(a(n),", "))                 

