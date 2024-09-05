\\ a(n) = Sum_{k=0..n} (Product_{j=0..k-1} (3*j-4)) * Stirling2(n,k).
a(n) = sum(k=0, n, prod(j=0, k-1, 3*j-4) * stirling(n, k, 2));
for(n=0, 19, print1(a(n),", "))                           


