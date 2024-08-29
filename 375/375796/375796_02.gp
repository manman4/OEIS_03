M=21;

\\ a(n) = n! * Sum_{k=0..floor(n/3)} (n-3*k)! * Stirling2(n-2*k,n-3*k)/(n-2*k)!.
a(n) = n! * sum(k=0, n\3, (n-3*k)!*stirling(n-2*k, n-3*k, 2)/(n-2*k)!);
for(n=0, M, print1(a(n), ", "));
