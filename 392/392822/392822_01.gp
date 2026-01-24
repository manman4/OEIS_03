M=12;

\\ A(n,k) = n! * Sum_{j=0..floor(n/(k+1))} (k*j)! * |Stirling1(n-j,k*j)|/(n-j)!.
a(n, k) = n!*sum(j=0, n\(k+1), (k*j)!*abs(stirling(n-j, k*j, 1))/(n-j)!);
for(n=0, M, for(k=0, n, print1(a(k,n-k), ", ")));