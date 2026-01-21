\\ a(n) = Sum_{k=0..n} Stirling2(n+2*k,3*k).
a(n) = sum(k=0, n, stirling(n+2*k, 3*k, 2));
for(n=0, 20, print1(a(n), ", "));

