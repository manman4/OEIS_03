M=23;

\\ a(n) = n! * Sum_{k=1..floor(n/2)} (-1)^(k-1) * (k-1)! * |Stirling1(n-k,k)|/(n-k)!.
a(n) = n! * sum(k=1, n\2, (-1)^(k-1) * (k-1)! * abs(stirling(n-k, k, 1)) / (n-k)!);
for(n=0, M, print1(a(n), ", "))
