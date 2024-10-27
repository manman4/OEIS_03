M=18;

\\ a(n) = 3 * n! * (3*n+2)! * Sum_{k=0..floor(n/2)} Stirling2(n-k,k)/( (n-k)! * (3*n-k+3)! ).
a(n) = 3 * n! * (3*n+2)! * sum(k=0, n\2, stirling(n-k,k,2)/((n-k)!*(3*n-k+3)!));
for(n=0, M, print1(a(n), ", ")) 