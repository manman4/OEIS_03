M=19;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/2)} (n+k)! * |Stirling1(n-k,k)|/(n-k)!.
a(n) = (1/(n+1)) * sum(k=0, n\2, (n+k)! * abs(stirling(n-k, k, 1))/(n-k)!);
for(n=0, M, print1(a(n),", "))   
