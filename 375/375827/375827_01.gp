M=20;

\\ a(n) = n! * Sum_{k=0..floor(n/3)} (n-2*k+1)^(k-1) * Stirling2(n-2*k,k)/(n-2*k)!.
a(n) = n! * sum(k=0, n\3, (n-2*k+1)^(k-1) * stirling(n-2*k, k, 2)/(n-2*k)!);
for(n=0, M, print1(a(n), ", "));
