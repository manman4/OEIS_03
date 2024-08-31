M=20;

\\ a(n) = n! * Sum_{k=0..floor(n/2)} (n-k)! * Stirling2(k,n-2*k)/( k! * (k+1)! ).
a(n) = n! * sum(k=0, n\2, (n-k)! * stirling(k, n-2*k, 2)/(k! * (k+1)!));
for(n=0, M, print1(a(n), ", "));
