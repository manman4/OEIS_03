\\ a(n) = Sum_{k=1..n} k! * k^(2*n-1) * Stirling1(n,k)
a(n) = sum(k=1, n, k! * k^(2*n-1) * stirling(n,k,1));
for(n=0, 13, print1(a(n),", "))      