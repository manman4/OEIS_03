\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} (2*(n-3*k))!/(n-3*k)! * (2*n-3*k)! * Stirling2(n-2*k,2*(n-3*k))/(n-2*k)!.
a(n) = sum(k=0, n\3, (2*(n-3*k))!/(n-3*k)! * (2*n-3*k)! * stirling(n-2*k, 2*(n-3*k), 2)/(n-2*k)!)/(n+1);
for(n=0, 21, print1(a(n), ", "));

