\\ a(n) = n * Sum_{k=1..n-1} 3^k * (k-1)! * Stirling2(n-1,k).
a(n) = n * sum(k=1, n-1, 3^k * (k-1)! * stirling(n-1, k, 2));
for(n=0, 18, print1(a(n),", "))

