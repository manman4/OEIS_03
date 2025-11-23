M=600;

\\ a(n) = Sum_{k>=0} (1/2)^(k+1) * |Stirling1(n+k,k)|.
a(n) = sum(k=0, M, (1/2.)^(k+1) * abs(stirling(n+k, k, 1)) );
for(n=0, 10, print1(a(n),", "));

a(n) = round( sum(k=0, M, (1/2.)^(k+1) * abs(stirling(n+k, k, 1)) ) );
for(n=0, 10, print1(a(n),", "));

