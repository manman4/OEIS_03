M=20;

\\ a(n) = n! * Sum_{k=0..floor(n/2)} (n-2*k)! * |Stirling1(n-k,n-2*k)|/(n-k)!.
a(n) = n! * sum(k=0, n\2, (n-2*k)!*abs(stirling(n-k, n-2*k, 1))/(n-k)!);
for(n=0, M, print1(a(n), ", "));
