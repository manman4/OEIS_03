M=22;

\\ a(n) = n! * Sum_{k=0..floor(n/2)} (n+1)^(n-2*k-1) * Stirling2(k,n-2*k)/k!.
a(n) = n!*sum(k=0, n\2, (n+1)^(n-2*k-1)*stirling(k, n-2*k, 2)/k!);
for(n=0, M, print1(a(n), ", "));