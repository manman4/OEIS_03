\\ a(n) = n! * Sum_{k=0..floor(n/2)} (2*n-3*k)!/(n-k+1)! * |Stirling1(k,n-2*k)|/k!.
a(n) = n! * sum(k=0, n\2, (2*n-3*k)! / (n-k+1)! * abs(stirling(k, n-2*k, 1)) / k!);
for(n=0, 21, print1(a(n), ", "));

