\\ a(n) = (1/(n+1)!) * Sum_{k=0..floor(n/2)} (2*k)!/k! * (n+k)! * Stirling2(n,2*k).
a(n) = sum(k=0, n\2, (2*k)!/k! * (n+k)! * stirling(n, 2*k, 2))/(n+1)!;
for(n=0, 19, print1(a(n), ", "));

