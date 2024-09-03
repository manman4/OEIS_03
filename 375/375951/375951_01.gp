M=17;

a008544(n) = prod(k=0, n-1, 3*k+2);
for(n=0, M, print1(a008544(n), ", "));

\\ a(n) = (1/2) * Sum_{k=0..n} A008544(k+1) * |Stirling1(n,k)|.
a(n) = sum(k=0, n, a008544(k+1) * abs(stirling(n,k,1))) / 2;
for(n=0, M, print1(a(n), ", "));
