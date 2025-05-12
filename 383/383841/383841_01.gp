\\ a(n) = Sum_{k=0..n} Stirling2(k+3,3) * Stirling2(n-k+3,3).
a(n) = sum(k=0, n, stirling(k+3,3,2) * stirling(n-k+3,3,2));
for(n=0, 25, print1(a(n),", "));                  

