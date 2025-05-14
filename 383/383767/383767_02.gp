\\ a(n) = Sum_{k=0..n} |Stirling1(n,k)| * Stirling2(k+n-1,n-1) for n > 0.
a(n) = if(n==0, 1, sum(k=0, n, abs(stirling(n, k, 1)) * stirling(n+k-1, n-1, 2))); 
for(n=0, 20, print1(a(n),", "))                   

