\\ a(n) = Sum_{k=1..floor(n/5)} (5*k)! * Stirling2(n,5*k)/(k * 120^k).
a(n) = sum(k=1, n\5, (5*k)! * stirling(n, 5*k, 2)/(k * 120^k));
for(n=5, 25, print1(a(n),", "))                 

