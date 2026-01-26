\\ a(n) = (n!)^2 * Sum_{k=0..floor(n/3)} (2*(n-3*k))!/((n-3*k)! * (3*k+1)!) * Stirling2(n-2*k,2*(n-3*k))/(n-2*k)!.
a(n) = n!^2 * sum(k=0, n\3, (2*(n-3*k))! / ((n-3*k)! * (3*k+1)!) * stirling(n-2*k, 2*(n-3*k), 2) / (n-2*k)! );
for(n=0, 21, print1(a(n), ", "));

