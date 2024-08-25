\\ a(n) = (1/2) * Sum_{k=0..n} 3^k * (k+2)! * |Stirling1(n,k)|.
a(n) = sum(k=0, n, 3^k * (k+2)! * abs(stirling(n, k, 1))) / 2;
for(n=0, 16, print1(a(n), ", "))
