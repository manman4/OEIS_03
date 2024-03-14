M=22;

\\ a(n) = n! * Sum_{j=0..n} Sum_{k=0..floor(j/2)} binomial(j-k,n-j-k) * Stirling1(j-k,k)/(j-k)!.
a(n) = n! * sum(j=0, n, sum(k=0, j\2, binomial(j-k,n-j-k) * stirling(j-k,k)/(j-k)!));
for(n=0, M, print1(a(n)", "))