\\ a(n) = n! * Sum_{k=0..floor(n/3)} A001147(k) * |Stirling1(n-2*k,k)|/(2^k*(n-2*k)!).
a001147(n) = prod(k=0, n-1, 2*k+1);
a(n) = n! * sum(k=0, n\2, a001147(k) * abs(stirling(n-2*k, k)) / (2^k * (n-2*k)!));
for(n=0, 22, print1(a(n), ", "))
