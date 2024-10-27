M=19;

\\ a(n) = 2 * n! * (2*n+1)! * Sum_{k=0..floor(n/2)} |Stirling1(n-k,k)|/( (n-k)! * (2*n-k+2)! ).
a(n) = 2 * n! * (2*n+1)! * sum(k=0, n\2, abs(stirling(n-k,k,1))/((n-k)!*(2*n-k+2)!));
for(n=0, M, print1(a(n), ", ")) 