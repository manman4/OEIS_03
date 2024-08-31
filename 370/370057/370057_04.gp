\\ a(n) = 3 * Sum_{k=0..n} (3*n+3)^(k-1) * |Stirling1(n,k)|.
a(n) = 3 * sum(k=0, n, (3*n+3)^(k-1)*abs(stirling(n, k, 1)));
for(n=0, 25, print1(a(n),", "))


