M=20;

\\ a(n) = n! * Sum_{j=0..floor(n/2)} Sum_{k=0..j} k! * binomial(j,n-2*j-k) * |Stirling1(j,k)|/j!.
a(n) = n! * sum(j=0, n\2, sum(k=0, j, k! * binomial(j,n-2*j-k) * abs(stirling(j,k,1))/j!));
for(n=0, M, print1(a(n)", "))