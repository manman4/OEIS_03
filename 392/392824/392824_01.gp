M=21;

\\ a(n) = n! * Sum_{k=0..floor(n/4)} (3*k)! * |Stirling1(n-k,3*k)|/(n-k)!.
a(n) = n!*sum(k=0, n\4, (3*k)!*abs(stirling(n-k, 3*k, 1))/(n-k)!);
for(n=0, M, print1(a(n), ", "));