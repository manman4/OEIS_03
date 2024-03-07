M=20;

\\ a(n) = (1/(n+1)) * Sum_{k=0..floor(n/3)} (n+k)! * |Stirling1(n-2*k,k)|/(n-2*k)!.
a(n) = (1/(n+1)) * sum(k=0, n\3, (n+k)! * abs(stirling(n-2*k, k, 1))/(n-2*k)!);
for(n=0, M, print1(a(n),", "))   
