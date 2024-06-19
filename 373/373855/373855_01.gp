\\ a(n) = Sum_{k=1..n} k! * k^(n-1) * |Stirling1(n,k)|.
a(n) = sum(k=1, n, k! * k^(n-1) * abs(stirling(n,k,1)));
for(n=0, 15, print1(a(n),", "))      