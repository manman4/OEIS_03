M=15;

\\ a(n) = A132393(3*n,2*n) = |Stirling1(3*n,2*n)|.
a(n) = abs(stirling(3*n,2*n,1));
for(n=0, M, print1(a(n),", "));
