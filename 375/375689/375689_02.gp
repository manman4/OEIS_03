\\ a(n) = n! * Sum_{k=0..floor(n/2)} (Product_{j=0..k-1} (3*j+2)) * |Stirling1(n-k,k)|/(n-k)!.
a(n) = n! * sum(k=0, n\2, prod(j=0, k-1, 3*j+2) * abs(stirling(n-k, k, 1))/(n-k)!);
for(n=0, 19, print1(a(n), ", "))
