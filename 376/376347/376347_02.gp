\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} (2*n-3*k)! * Stirling2(k,n-3*k)/k!.
a(n) = (1/(n+1)) * sum(k=0, n\3, (2*n-3*k)! * stirling(k, n-3*k, 2) / k!);
for(n=0, 23, print1(a(n), ", "))