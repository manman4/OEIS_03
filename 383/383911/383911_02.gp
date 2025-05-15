\\ a(n) = Sum_{k=0..4} |Stirling1(5,k+1)| * Stirling2(k+n,4).
b(n) = sum(k=0, 4, abs(stirling(5, k+1, 1)) * stirling(k+n, 4, 2));
for(n=0, 24, print1(b(n),", "));                  

\\ a(n) = 10*a(n-1) - 35*a(n-2) + 50*a(n-3) - 24*a(n-4) for n > 4.
print("out");
a(n) = if(n<4, b(n), 10*a(n-1) - 35*a(n-2) + 50*a(n-3) - 24*a(n-4));
for(n=0, 25, print1(a(n)-b(n),", "));
print("OK");
a(n) = if(n<5, b(n), 10*a(n-1) - 35*a(n-2) + 50*a(n-3) - 24*a(n-4));
for(n=0, 25, print1(a(n)-b(n),", "));