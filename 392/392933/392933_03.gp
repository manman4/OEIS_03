\\ a(n) = (n!)^2 * Sum_{k=0..floor(n/2)} 1/(2*k+1)! * |Stirling1(k,n-2*k)|/k!.
a(n) = n!^2 * sum(k=0, n\2, 1/(2*k+1)! * abs(stirling(k, n-2*k, 1)) / k!);
for(n=0, 21, print1(a(n), ", "));

