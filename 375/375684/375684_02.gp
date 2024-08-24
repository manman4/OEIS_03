\\ a(n) = n! * Sum_{k=0..floor(n/2)} (-1)^k * k! * |Stirling1(n-k,k)|/(n-k)!.
a(n) = n! * sum(k=0, n\2, (-1)^k * k! * abs(stirling(n-k, k, 1))/(n-k)!);
for(n=0, 22, print1(a(n), ", "))
