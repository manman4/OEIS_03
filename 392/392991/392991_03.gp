\\ a(n) = (n!)^2 * Sum_{k=0..floor(n/2)} (2*(n-2*k))!/((n-2*k)! * (2*k+1)!) * Stirling2(k,2*(n-2*k))/k!.
a(n) = (n!)^2 * sum(k=0, n\2, (2*(n-2*k))!/((n-2*k)! * (2*k+1)!) * stirling(k, 2*(n-2*k), 2) / k!);
for(n=0, 22, print1(a(n), ", "));

