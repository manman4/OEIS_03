M=19;

\\ a(n) = n! * Sum_{k=0..floor(n/2)} (2*n-2*k)! * |Stirling1(n-k,k)|/( (n-k)! * (2*n-3*k+1)! ).
a(n) = n! * sum(k=0, n\2, (2*n-2*k)! * abs(stirling(n-k,k,1))/( (n-k)! * (2*n-3*k+1)! ));
for(n=0, M, print1(a(n)", "))