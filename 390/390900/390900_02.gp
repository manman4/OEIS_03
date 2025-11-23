M=600;

\\ a(n) = (1/4) * Sum_{k>=0} (3/4)^k * |Stirling1(n+k,k)|.
a(n) = 1/4 * sum(k=0, M, (3/4.)^k * abs(stirling(n+k, k, 1)) );
for(n=0, 10, print1(a(n),", "));

a(n) = round( 1/4 * sum(k=0, M, (3/4)^k * abs(stirling(n+k, k, 1)) ) );
for(n=0, 10, print1(a(n),", "));

