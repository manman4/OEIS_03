M=23;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/4)} (2*n-4*k)! * |Stirling1(k,n-4*k)|/k!.
a(n) = sum(k=0, n\4, (2*n-4*k)!*abs(stirling(k, n-4*k, 1))/k!)/(n+1);
for(n=0, M, print1(a(n), ", "));

