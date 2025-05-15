\\ a(n) = Sum_{k=0..4} |Stirling1(4,k)| * Stirling2(k+n,4).
b(n) = sum(k=0, 4, abs(stirling(4, k, 1)) * stirling(k+n, 4, 2));
for(n=0, 22, print1(b(n),", "));                  

\\ a(n) = 10*a(n-1) - 35*a(n-2) + 50*a(n-3) - 24*a(n-4).
a(n) = if(n<4, b(n), 10*a(n-1) - 35*a(n-2) + 50*a(n-3) - 24*a(n-4));
for(n=0, 25, print1(a(n)-b(n),", "));
