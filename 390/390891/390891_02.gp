M=600;

\\ a(n) = (1/3) * Sum_{k>=0} (2/3)^k * Stirling2(n+k,k).
a(n) = 1/3 * sum(k=0, M, (2/3.)^k * stirling(n+k, k, 2) );
for(n=0, 10, print1(a(n),", "));

a(n) = round( 1/3 * sum(k=0, M, (2/3)^k * stirling(n+k, k, 2) ) );
for(n=0, 10, print1(a(n),", "));

