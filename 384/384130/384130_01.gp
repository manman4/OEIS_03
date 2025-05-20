M=14;

\\ a(n) = A132393(4*n,3*n) = |Stirling1(4*n,3*n)|.
a(n) = abs(stirling(4*n,3*n,1));
for(n=0, M, print1(a(n),", "));
