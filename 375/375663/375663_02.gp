\\ a(n) = (n!/2) * Sum_{k=0..floor(n/3)} (k+2)! * Stirling2(n-2*k,k)/(n-2*k)!.
a(n) = n!/2 * sum(k=0, n\3, (k+2)! * stirling(n-2*k, k, 2)/(n-2*k)!);
for(n=0, 21, print1(a(n), ", "))
