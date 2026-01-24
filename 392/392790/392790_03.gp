\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/4)} (2*n-4*k)! * Stirling2(n-3*k,n-4*k)/(n-3*k)!.
a(n) = sum(k=0, n\4, (2*n-4*k)! * stirling(n-3*k, n-4*k, 2)/(n-3*k)!)/(n+1);
for(n=0, 18, print1(a(n), ", "));

