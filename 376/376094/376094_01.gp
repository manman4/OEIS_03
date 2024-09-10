\\ a(n) = n! * Sum_{k=1..n} (3*n)^(k-1) * binomial(n-1,k-1)/k!.
a(n) = n! * sum(k=1, n, (3*n)^(k-1) * binomial(n-1,k-1)/k!);
for(n=0, 16, print1(a(n), ", "))

