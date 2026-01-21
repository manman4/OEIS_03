\\ a(n) = Sum_{k=0..floor(n/2)} Stirling2(n+k,3*k).
a(n) = sum(k=0, n\2, stirling(n+k, 3*k, 2));
for(n=0, 22, print1(a(n), ", "));

