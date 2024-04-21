\\ a(n) = n! * Sum_{k=0..n} (k+1)^(k-1) * 3^(n-k) * binomial(k/3,n-k)/k!.
a(n) = n! * sum(k=0, n, (k+1)^(k-1) * 3^(n-k) * binomial(k/3,n-k)/k!);
for(n=0, 18, print1(a(n), ", "))

