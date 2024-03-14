M=18;

\\ a(n) = n! * Sum_{k=0..floor(n/2)} (2*n+1)^(k-1) * |Stirling1(n-k,k)|/(n-k)!.
a(n) = n! * sum(k=0, n\2, (2*n+1)^(k-1) * abs(stirling(n-k,k,1))/(n-k)!);
for(n=0, M, print1(a(n)", "))