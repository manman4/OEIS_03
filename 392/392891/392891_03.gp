\\ a(n) = (n!)^2 * Sum_{k=0..floor(n/2)} (2*(n-2*k))!/((n-2*k)! * (2*k+1)!) * Stirling2(n-k,2*(n-2*k))/(n-k)!.
a(n) = n!^2 * sum(k=0, n\2, (2*(n-2*k))! / ((n-2*k)! * (2*k+1)!) * stirling(n-k, 2*(n-2*k), 2) / (n-k)! );
for(n=0, 21, print1(a(n), ", "));

