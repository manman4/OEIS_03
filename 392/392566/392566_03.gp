\\ a(n) = n! * Sum_{k=0..floor(n/2)} (n+1)^(n-k-1) * 3^(n-2*k) * binomial(n-k,k)/(n-k)!.
a(n) = n! * sum(k=0, n\2, (n+1)^(n-k-1) * 3^(n-2*k) * binomial(n-k,k)/(n-k)!);
for(n=0, 17, print1(a(n),", "));