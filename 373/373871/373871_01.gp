\\ a(n) = Sum_{k=1..n} k! * k^(n-3) * Stirling2(n,k).
a(n) = sum(k=1, n, k! * k^(n-3) * stirling(n,k,2));
for(n=0, 16, print1(a(n),", "))      