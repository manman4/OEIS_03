\\ a(n) = n! * Sum_{k=0..n} (-1)^(n-k) * binomial(k+3,4) / k!.
a365586(n) = n! * sum(k=0, n, (-1)^(n-k) * binomial(k+3,4) / k!);
for(n=0, 21, print1(a365586(n), ", "));
a(n) = n! + (-1)^n * a365586(n);
for(n=0, 22, print1(a(n), ", "));
