\\ a(n) = (n!)^2 * Sum_{k=0..floor(n/3)} 1/(3*k+1)! * Stirling2(k,n-3*k)/k!.
a(n) = (n!)^2 * sum(k=0, n\3, 1/(3*k+1)! * stirling(k, n-3*k, 2) / k!);
for(n=0, 22, print1(a(n), ", "));

