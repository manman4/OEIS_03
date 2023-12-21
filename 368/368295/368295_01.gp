\\ a(n) = n! * Sum_{k=0..n} (3*(n-k)-1)^k / k!.
a(n) = n! * sum(k=0, n, (3*(n-k)-1)^k / k!);
for(n=0, 18, print1(a(n), ", "));
