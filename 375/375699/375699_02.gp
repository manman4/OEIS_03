\\ a(n) = n! * Sum_{k=0..floor(n/4)} (Product_{j=0..k-1} (6*j+1)) * |Stirling1(n-3*k,k)|/(6^k*(n-3k)!).
a(n) = n! * sum(k=0, n\4, prod(j=0, k-1, 6*j+1) * abs(stirling(n-3*k, k)) / (6^k * (n-3*k)!));
for(n=0, 22, print1(a(n), ", "))
