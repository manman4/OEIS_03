M=17;

\\ a(n) = (3/(3*n+3)!) * Sum_{k=0..n} (3*n+k+2)! * |Stirling1(n,k)|.
a(n) = 3/(3*n+3)!*sum(k=0, n, (3*n+k+2)!*abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));