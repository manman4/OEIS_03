\\ a(n) = Sum_{k=1..n} (-1)^(n-k) * k! * k^(n+1) * Stirling2(n,k).
a(n) = sum(k=1, n, (-1)^(n-k) * k! * k^(n+1) * stirling(n,k,2));
for(n=0, 15, print1(a(n),", "))      