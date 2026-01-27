\\ a(n) = n! * Sum_{k=0..floor(n/2)} (3*n-4*k)!/(2*n-2*k+1)! * |Stirling1(n-k,n-2*k)|/(n-k)!.
a(n) = n! * sum(k=0, n\2, (3*n-4*k)! / (2*n-2*k+1)! * abs(stirling(n-k, n-2*k, 1)) / (n-k)!);
for(n=0, 18, print1(a(n), ", "));

