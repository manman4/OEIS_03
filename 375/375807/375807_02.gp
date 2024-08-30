M=19;

\\ a(n) = (n!/2) * Sum_{k=0..floor(n/2)} (n-2*k+2)! * |Stirling1(n-k,n-2*k)|/(n-k)!.
a(n) = (n!/2) * sum(k=0, n\2, (n-2*k+2)!*abs(stirling(n-k, n-2*k))/(n-k)!);
for(n=0, M, print1(a(n), ", "));
