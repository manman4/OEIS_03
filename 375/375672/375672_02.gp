\\ a(n) = (n!/2) * Sum_{k=0..floor(n/2)} (k+2)! * |Stirling1(n-k,k)|/(n-k)!.
a(n) = n!/2 * sum(k=0, n\2, (k+2)! * abs(stirling(n-k, k, 1))/(n-k)!);
for(n=0, 20, print1(a(n), ", "))
