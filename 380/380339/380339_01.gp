M=24;

\\ a(n) = n! * Sum_{k=1..floor(n/3)} (-1)^(k-1) * (k-1)! * |Stirling1(n-2*k,k)|/(2^k * (n-2*k)!).
a(n) = n! * sum(k=1, n\3, (-1)^(k-1) * (k-1)! * abs(stirling(n-2*k, k, 1)) / (2^k * (n-2*k)!));
for(n=0, M, print1(a(n), ", "))
