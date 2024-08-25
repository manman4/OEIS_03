\\ a(n) = Sum_{k=0..n} 3^k * (k+1)! * |Stirling1(n,k)|.
a(n) = sum(k=0, n, 3^k * (k+1)! * abs(stirling(n, k, 1)));
for(n=0, 17, print1(a(n), ", "))
