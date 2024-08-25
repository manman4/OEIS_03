a001147(n) = prod(k=0, n-1, 2*k+1);

\\ a(n) = n! * Sum_{k=0..floor(n/2)} A001147(k) * Stirling2(n-k,k)/(n-k)!.
a(n) = n! * sum(k=0, n\2, a001147(k) * stirling(n-k, k, 2)/(n-k)!);
for(n=0, 20, print1(a(n), ", "))
