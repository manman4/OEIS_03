M=16;

\\ a(n) = Sum_{k=1..n} (4*k-2)!/(3*k-1)! * Stirling2(n,k).
a(n) = sum(k=1, n, (4*k-2)!/(3*k-1)! * stirling(n,k,2));
for(n=0, M, print1(a(n)", "))