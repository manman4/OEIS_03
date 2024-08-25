\\ a(n) = n! * Sum_{k=0..floor(n/3)} A001147(k) * Stirling2(n-2*k,k)/(2^k*(n-2*k)!).
a(n) = n! * sum(k=0, n\3, prod(j=0, k-1, 2*j+1) * stirling(n-2*k, k, 2) / (2^k * (n-2*k)!));
for(n=0, 22, print1(a(n), ", "))
