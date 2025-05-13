\\ a(n) = Sum_{k=0..n} Stirling2(n+k-1,n-1) * Stirling2(2*n-k-1,n-1) for n > 0.
a(n) = if(n==0, 1, sum(k=0, n, stirling(n+k-1, n-1, 2) * stirling(2*n-k-1, n-1, 2)));
for(n=0, 17, print1(a(n),", "))                   

