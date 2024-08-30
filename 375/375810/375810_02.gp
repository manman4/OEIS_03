M=20;

\\ a(n) = n! * Sum_{k=0..floor(n/2)} (n-2*k+1)! * Stirling2(n-k,n-2*k)/(n-k)!.
a(n) = n! * sum(k=0, n\2, (n-2*k+1)!*stirling(n-k, n-2*k, 2)/(n-k)!);
for(n=0, M, print1(a(n), ", "));
