M=21;

\\ a(n) = n! * Sum_{j=0..floor(n/3)} Sum_{k=0..j} binomial(j,n-3*j-k) * |Stirling1(j,k)|/j!.
a(n) = n! * sum(j=0, n\3, sum(k=0, j, binomial(j,n-3*j-k) * abs(stirling(j,k))/j!));
for(n=0, M, print1(a(n)", "))