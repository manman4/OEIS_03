M=20;

\\ a(n) = (n!)^2 * Sum_{k=0..floor(n/2)} Stirling2(n-k,k)/( (n-k)! * (n-k+1)! ).
a(n) =(n!)^2 * sum(k=0, n\2, stirling(n-k,k,2)/( (n-k)! * (n-k+1)! ));
for(n=0, M, print1(a(n)", "))