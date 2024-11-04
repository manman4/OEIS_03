M=19;

\\ a(n) = 3 * n! * Sum_{k=0..floor(n/2)} (3*n-3*k+2)! * |Stirling1(n-k,k)|/( (n-k)! * (3*n-4*k+3)! ).
a(n) = 3 * n! * sum(k=0, n\2, (3*n-3*k+2)! * abs(stirling(n-k,k,1))/((n-k)! * (3*n-4*k+3)!));
for(n=0, M, print1(a(n), ", ")) 