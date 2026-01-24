M=21;

\\ a(n) = n! * Sum_{k=0..floor(n/3)} (2*k)! * |Stirling1(n-k,2*k)|/(n-k)!.
a(n) = n!*sum(k=0, n\3, (2*k)!*abs(stirling(n-k, 2*k, 1))/(n-k)!);
for(n=0, M, print1(a(n), ", "));