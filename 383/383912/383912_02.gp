\\ a(n) = Sum_{k=0..3} |Stirling1(3,k)| * Stirling2(k+n,3).
b(n) = sum(k=0, 3, abs(stirling(3, k, 1)) * stirling(k+n, 3, 2));   
for(n=0, 22, print1(b(n),", "));                  

\\ a(n) = 6*a(n-1) - 11*a(n-2) + 6*a(n-3).
a(n) = if(n<3, b(n), 6*a(n-1) - 11*a(n-2) + 6*a(n-3));
for(n=0, 25, print1(a(n)-b(n),", "));
