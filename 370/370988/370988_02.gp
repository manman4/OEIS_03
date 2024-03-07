M=19;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} (n+k)! * Stirling2(n-k,k)/(n-k)!.
a(n) = (1/(n+1)) * sum(k=0, n\2, (n+k)! * stirling(n-k, k, 2)/(n-k)!);
for(n=0, M, print1(a(n),", "))   
