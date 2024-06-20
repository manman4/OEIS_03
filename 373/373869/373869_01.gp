\\ a(n) = Sum_{k=1..n} k! * k^(n-3) * Stirling1(n,k).
a(n) = sum(k=1, n, k! * k^(n-3) * stirling(n,k,1));
for(n=0, 17, print1(a(n),", "))      