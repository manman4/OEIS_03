\\ a(n) = n! * Sum_{k=0..floor(n/2)} (-1)^k * k! * Stirling2(n-k,k)/(n-k)!.
a(n) = n! * sum(k=0, n\2, (-1)^k * k! * stirling(n-k, k, 2)/(n-k)!);
for(n=0, 21, print1(a(n), ", "))
