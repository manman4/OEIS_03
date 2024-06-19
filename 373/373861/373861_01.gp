\\ a(n) = Sum_{k=0..n} k^(2*n) * |Stirling1(n,k)|.
a(n) = sum(k=0, n, k^(2*n)*abs(stirling(n,k,1)));
for(n=0, 13, print1(a(n),", "))      