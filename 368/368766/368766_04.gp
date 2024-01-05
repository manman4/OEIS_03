\\ a(n) = n! * Sum_{k=0..n} (-1)^(n-k) * binomial(k+1,2) / k!.
a009574(n) = n! * sum(k=0, n, (-1)^(n-k) * binomial(k+1,2) / k!);
for(n=0, 21, print1(a009574(n), ", "));
a(n) = n! + (-1)^n * a009574(n);
for(n=0, 22, print1(a(n), ", "));
