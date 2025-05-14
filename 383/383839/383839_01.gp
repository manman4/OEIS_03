\\ a(n) = Sum_{k=0..n} |Stirling1(n,k)| * Stirling2(k+n,n).
a(n) = sum(k=0, n, abs(stirling(n, k, 1)) * stirling(k+n, n, 2));
for(n=0, 17, print1(a(n),", "))                   

