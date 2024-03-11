M=18;

\\ a(n) = n! * Sum_{k=0..floor(n/2)} (n+2*k)! * Stirling2(n-k,k)/( (n-k)! * (n+k+1)! ).
a(n) = n!* sum(k=0, n\2, (n+2*k)! * stirling(n-k,k,2)/( (n-k)! * (n+k+1)! ));
for(n=0, M, print1(a(n)", "))