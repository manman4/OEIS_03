M=22;

\\ a(n) = Sum_{k=0..floor((n+1)/2)} (n-k)!/(n-2*k+1)! * Stirling2(n,k).
a(n) = sum(k=0, (n+1)\2, (n-k)!/(n-2*k+1)! * stirling(n,k,2));
for(n=0, M, print1(a(n), ", ")) 