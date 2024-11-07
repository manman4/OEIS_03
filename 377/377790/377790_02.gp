M=17;

\\ a(n) = n! * Sum_{k=0..n} 3^k * |Stirling1(n,k)|/(n-k+1)!.
a(n) = n! * sum(k=0, n, 3^k * abs(stirling(n,k,1))/(n-k+1)!);
for(n=0, M, print1(a(n),", "));

