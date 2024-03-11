M=20;

\\ a(n) = n! * Sum_{k=0..floor(n/3)} |Stirling1(n-2*k,k)|/(n-3*k+1)!.
a(n) = n! * sum(k=0, n\3, abs(stirling(n-2*k,k,1))/(n-3*k+1)!);
for(n=0, M, print1(a(n)", "))