M=19;

\\ a(n) = n! * Sum_{k=0..floor(n/2)} (3*n-3*k)! * |Stirling1(n-k,k)|/( (n-k)! * (3*n-4*k+1)! ).
a(n) = n! * sum(k=0, n\2, (3*n-3*k)! * abs(stirling(n-k,k,1))/((n-k)! * (3*n-4*k+1)!));
for(n=0, M, print1(a(n), ", ")) 