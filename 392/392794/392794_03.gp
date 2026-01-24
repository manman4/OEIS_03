\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} (2*(n-2*k))!/(n-2*k)! * (2*n-2*k)! * Stirling2(n-k,2*(n-2*k))/(n-k)!.
a(n) = sum(k=0, n\2, (2*(n-2*k))!/(n-2*k)! * (2*n-2*k)! * stirling(n-k, 2*(n-2*k), 2)/(n-k)!)/(n+1);
for(n=0, 20, print1(a(n), ", "));

