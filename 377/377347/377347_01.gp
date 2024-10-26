M=20;

\\ a(n) = Sum_{k=0..floor((2*n+1)/3)} (2*n-2*k)!/(2*n-3*k+1)! * Stirling2(n,k).
a(n) = sum(k=0, (2*n+1)\3, (2*n-2*k)!/(2*n-3*k+1)! * stirling(n,k,2));
for(n=0, M, print1(a(n), ", ")) 