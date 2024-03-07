M=20;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/4)} (n+k)! * |Stirling1(n-3*k,k)|/(n-3*k)!.
a(n) = (1/(n+1)) * sum(k=0, n\4, (n+k)! * abs(stirling(n-3*k, k, 1))/(n-3*k)!);
for(n=0, M, print1(a(n),", "))   
