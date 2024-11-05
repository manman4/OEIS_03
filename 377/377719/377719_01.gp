M=20;

\\ a(n) = 2 * n! * Sum_{k=0..floor(n/2)} (2*k+1)! * Stirling2(n-k,k)/( (n-k)! * (k+2)! ).
a(n) = 2 * n! * sum(k=0, floor(n/2), (2*k+1)! * stirling(n-k,k,2)/( (n-k)! * (k+2)! ));
for(n=0, M, print1(a(n), ", ")) 