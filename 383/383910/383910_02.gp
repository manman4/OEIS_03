\\ a(n) = Sum_{k=0..3} |Stirling1(4,k+1)| * Stirling2(k+n,3).
b(n) = sum(k=0, 3, abs(stirling(4, k+1, 1)) * stirling(k+n, 3, 2));
for(n=0, 26, print1(b(n),", "));                  

\\ a(n) = 6*a(n-1) - 11*a(n-2) + 6*a(n-3) for n > 3.
print("out");
a(n) = if(n<3, b(n), 6*a(n-1) - 11*a(n-2) + 6*a(n-3));
for(n=0, 25, print1(a(n)-b(n),", "));
print("OK");
a(n) = if(n<4, b(n), 6*a(n-1) - 11*a(n-2) + 6*a(n-3));
for(n=0, 25, print1(a(n)-b(n),", "));