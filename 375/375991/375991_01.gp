\\ a(n) = Sum_{k=0..n} (Product_{j=0..k-1} (2*j-3)) * Stirling2(n,k).
a(n) = sum(k=0, n, prod(j=0, k-1, 2*j-3) * stirling(n, k, 2));
for(n=0, 20, print1(a(n),", "))                           


