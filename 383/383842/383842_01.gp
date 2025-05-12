\\ a(n) = Sum_{k=0..n} Stirling2(k+4,4) * Stirling2(n-k+4,4).
b(n) = sum(k=0, n, stirling(k+4,4,2) * stirling(n-k+4,4,2));
for(n=0, 22, print1(b(n),", "));                  

\\ a(n) = 20*a(n-1) - 170*a(n-2) + 800*a(n-3) - 2273*a(n-4) + 3980*a(n-5) - 4180*a(n-6) + 2400*a(n-7) - 576*a(n-8).
a(n) = if(n<8, b(n), 20*a(n-1) - 170*a(n-2) + 800*a(n-3) - 2273*a(n-4) + 3980*a(n-5) - 4180*a(n-6) + 2400*a(n-7) - 576*a(n-8));
for(n=0, 25, print1(a(n)-b(n),", "));
