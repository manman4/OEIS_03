M=18;

\\ a(n) = 3 * Sum_{k=0..floor((3*n+3)/4)} (3*n-3*k+2)!/(3*n-4*k+3)! * Stirling2(n,k).
a(n) = 3 * sum(k=0, (3*n+3)\4, (3*n-3*k+2)!/(3*n-4*k+3)! * stirling(n,k,2));
for(n=0, M, print1(a(n), ", ")) 