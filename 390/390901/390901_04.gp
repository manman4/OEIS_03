M=600;

\\ a(n) = (1/2) * Sum_{k>=0} (2/3)^(n+k) * |Stirling1(n-1+k,k)|.
a(n) = 1/2 * sum(k=0, M, (2/3.)^(n+k) * abs(stirling(n-1+k, k, 1)) );
for(n=1, 10, print1(a(n),", "));

a(n) = round( 1/2 * sum(k=0, M, (2/3)^(n+k) * abs(stirling(n-1+k, k, 1)) ) );
for(n=1, 10, print1(a(n),", "));

