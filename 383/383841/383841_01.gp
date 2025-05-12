\\ a(n) = Sum_{k=0..n} Stirling2(k+3,3) * Stirling2(n-k+3,3).
b(n) = sum(k=0, n, stirling(k+3,3,2) * stirling(n-k+3,3,2));
for(n=0, 25, print1(b(n),", "));                  

\\ a(n) = 12*a(n-1) - 58*a(n-2) + 144*a(n-3) - 193*a(n-4) + 132*a(n-5) - 36*a(n-6).
a(n) = if(n<6, b(n), 12*a(n-1) - 58*a(n-2) + 144*a(n-3) - 193*a(n-4) + 132*a(n-5) - 36*a(n-6));
for(n=0, 25, print1(a(n)-b(n),", "));
