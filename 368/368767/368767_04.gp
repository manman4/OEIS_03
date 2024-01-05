\\ a(n) = n! * Sum_{k=0..n} (-1)^(n-k) * binomial(k+2,3) / k!.
a365585(n) = n! * sum(k=0, n, (-1)^(n-k) * binomial(k+2,3) / k!);
for(n=0, 21, print1(a365585(n), ", "));
a(n) = n! + (-1)^n * a365585(n);
for(n=0, 22, print1(a(n), ", "));
