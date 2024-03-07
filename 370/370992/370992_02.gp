M=22;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/4)} (n+k)! * Stirling2(n-3*k,k)/(6^k * (n-3*k)!).
a(n) = (1/(n+1)) * sum(k=0, n\4, (n+k)! * stirling(n-3*k, k, 2)/(6^k * (n-3*k)!));
for(n=0, M, print1(a(n),", "))   
