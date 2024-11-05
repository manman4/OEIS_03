M=19;

\\ a(n) = 3 * n! * Sum_{k=0..floor(n/2)} (3*k+2)! * Stirling2(n-k,k)/( (n-k)! * (2*k+3)! ).
a(n) = 3 * n! * sum(k=0, floor(n/2), (3*k+2)! * stirling(n-k,k,2)/( (n-k)! * (2*k+3)! ));
for(n=0, M, print1(a(n), ", ")) 