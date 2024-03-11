M=19;

\\ a(n) = (n!)^2 * Sum_{k=0..floor(n/2)} |Stirling1(n-k,k)|/( (n-k)! * (n-k+1)! ).
a(n) = (n!)^2 * sum(k=0, n\2, abs(stirling(n-k,k,1))/( (n-k)! * (n-k+1)! ));
for(n=0, M, print1(a(n)", "))