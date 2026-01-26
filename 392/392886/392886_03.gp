\\ a(n) = (n!)^2 * Sum_{k=0..floor(n/3)} 1/(3*k+1)! * |Stirling1(n-2*k,n-3*k)|/(n-2*k)!.
a(n) = n!^2 * sum(k=0, n\3, 1/(3*k+1)! * abs(stirling(n-2*k, n-3*k, 1)) / (n-2*k)! );
for(n=0, 20, print1(a(n), ", "));

