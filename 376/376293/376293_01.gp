\\ a(n) = (n!)^2 * Sum_{k=0..floor(n/4)} Stirling2(n-3*k,k)/( (n-3*k)! * (n-k+1)! ).
a(n) = n!^2 * sum(k=0, n\4, stirling(n-3*k,k,2)/( (n-3*k)! * (n-k+1)! ));
for(n=0, 20, print1(a(n), ", "))

