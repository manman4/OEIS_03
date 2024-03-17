M=19;

\\ a(n) = n! * Sum_{k=0..floor(n/2)} (2*n-k)! * Stirling2(n-k,k)/( (n-k)! * (2*n-2*k+1)! ).
a(n) = n! * sum(k=0, n\2, (2*n-k)! * stirling(n-k,k,2)/(n-k)!/(2*n-2*k+1)!);
for(n=0, M, print1(a(n)", "))