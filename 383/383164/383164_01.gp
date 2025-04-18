M=20;

\\ a(n) = Sum{k=3..n} 2^(n-k) * Stirling2(n,k) * |Stirling1(k,3)|.
a(n) = sum(k=3, n, 2^(n-k) * stirling(n,k,2) * abs(stirling(k,3,1)));
for(n=0, M, print1(a(n), ", "));

\\ Column k=3 of A383149.
T(n, k) = sum(j=k, n, 2^(n-j)*stirling(n, j, 2)*abs(stirling(j, k, 1)));
for(n=0, 50, print1(a(n)-T(n, 3), ", "));
