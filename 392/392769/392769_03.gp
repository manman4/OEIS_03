\\ a(n) = (1/(n+1)!) * Sum_{k=0..floor(n/3)} (3*k)!/k! * (n+k)! * Stirling2(n,3*k).
a(n) = sum(k=0, n\3, (3*k)!/k! * (n+k)! * stirling(n, 3*k, 2))/(n+1)!;
for(n=0, 20, print1(a(n), ", "));

