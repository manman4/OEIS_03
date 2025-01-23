\\ a(n) = Sum_{k=1..floor(n/2)} (-1)^(k-1) * (2*k)! * Stirling1(n,2*k)/(k * 2^k).
a(n) = sum(k=1, n\2, (-1)^(k-1) * (2*k)! * stirling(n, 2*k, 1)/(k * 2^k));
for(n=2, 25, print1(a(n),", "))                 

