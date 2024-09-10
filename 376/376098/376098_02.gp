\\ a(n) = n! * Sum_{k=1..n} (2*k)^(k-1) * binomial(n-1,k-1)/k!.
a(n) = n! * sum(k=1, n, 2^(k-1) * k^(k-1) * binomial(n-1,k-1)/k!);
for(n=0, 17, print1(a(n), ", "))

\\ a352448(n) = n! * Sum_{k=0..n} 2^k * (k+1)^(k-1) * binomial(n,k)/k!.
a352448(n) = n! * sum(k=0, n, 2^k * (k+1)^(k-1) * binomial(n,k)/k!);
for(n=1, 50, print1(a(n) - n*a352448(n-1), ", "))


