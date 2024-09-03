M=17;

a001147(n) = prod(k=0, n-1, 2*k+1);
for(n=0, M, print1(a001147(n), ", "));

\\ a(n) = (1/3) * Sum_{k=0..n} A001147(k+2) * |Stirling1(n,k)|.
a(n) = sum(k=0, n, a001147(k+2) * abs(stirling(n, k, 1))) / 3;
for(n=0, M, print1(a(n), ", "));
