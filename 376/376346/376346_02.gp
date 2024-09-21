\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} (2*n-3*k)! * |Stirling1(k,n-3*k)|/k!.
a(n) = sum(k=0, n\3, (2*n-3*k)! * abs(stirling(k, n-3*k, 1)) / k!)/(n+1);
for(n=0, 23, print1(a(n), ", "))