M=20;

\\ T(n,k) = Sum_{j=k..n} |Stirling1(n,j)| * Stirling1(j,k).
T(n,k) = sum(j=k, n, abs(stirling(n,j,1)) * stirling(j,k,1));
for(n=0, M, for(k=0, n, print1(T(n,k),", ")));


