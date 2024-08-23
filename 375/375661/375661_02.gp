\\ a(n) = (n!/2) * Sum_{k=0..floor(n/2)} (k+2)! * Stirling2(n-k,k)/(n-k)!.
a(n) = n!/2 * sum(k=0, n\2, (k+2)! * stirling(n-k, k, 2)/(n-k)!);
for(n=0, 20, print1(a(n), ", "))
