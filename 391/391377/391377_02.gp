M=22;

\\ a(n) = n! * Sum_{k=0..floor(n/4)} k^k * |Stirling1(n-3*k,k)|/(n-3*k)!.
a(n) = n! * sum(k=0, n\4, k^k * abs(stirling(n-3*k,k,1))/(n-3*k)!);
for(n=0, M, print1(a(n),", "));
