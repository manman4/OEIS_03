M=600;

\\ a(n) = Sum_{k>=0} (1/2)^(k+1) * Stirling2(n+k,k).
a(n) = sum(k=0, M, (1/2.)^(k+1) * stirling(n+k, k, 2) );
for(n=0, 10, print1(a(n),", "));

a(n) = round( sum(k=0, M, (1/2.)^(k+1) * stirling(n+k, k, 2) ) );
for(n=0, 10, print1(a(n),", "));

