\\ a(n) = Sum_{k=1..floor(n/3)} (3*k)! * Stirling2(n,3*k)/(k * 6^k).
a(n) = sum(k=1, n\3, (3*k)! * stirling(n, 3*k, 2)/(k * 6^k));
for(n=3, 25, print1(a(n),", "))                 

