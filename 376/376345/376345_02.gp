\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} (2*n-2*k)! * Stirling2(k,n-2*k)/k!.
a(n) = (1/(n+1)) * sum(k=0, n\2, (2*n-2*k)! * stirling(k, n-2*k, 2) / k!);
for(n=0, 22, print1(a(n), ", "))