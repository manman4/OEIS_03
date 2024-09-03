M=16;

a000407(n) = (2*n+1)!/n!;
for(n=0, M, print1(a000407(n), ", "));

\\ a(n) = Sum_{k=0..n} A000407(k) * |Stirling1(n,k)|.
a(n) = sum(k=0, n, a000407(k) * abs(stirling(n, k, 1)));
for(n=0, M, print1(a(n), ", "));
