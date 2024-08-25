\\ a(n) = n! * Sum_{k=0..floor(n/2)} (Product_{j=0..k-1} (3*j+1)) * Stirling2(n-k,k)/(n-k)!.
a(n) = n! * sum(k=0, n\2, prod(j=0, k-1, 3*j+1) * stirling(n-k, k, 2)/(n-k)!);
for(n=0, 19, print1(a(n), ", "))
