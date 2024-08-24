a001147(n) = prod(k=0, n-1, 2*k+1);
for(n=0, 20, print1(a001147(n), ", "))

\\ a(n) = n! * Sum_{k=0..floor(n/2)} A001147(k) * |Stirling1(n-k,k)|/(n-k)!.
a(n) = n! * sum(k=0, n\2, a001147(k) * abs(stirling(n-k, k, 1))/(n-k)!);
for(n=0, 20, print1(a(n), ", "))
