M=18;

\\ a(n) = n! * Sum_{k=0..n} (k+1)^(n-k) * binomial(k+3,3)/(n-k)!.
a(n) = n! * sum(k=0, n, (k+1)^(n-k) * binomial(k+3,3) / (n-k)!);
for(n=0, M, print1(a(n),", "));

\\ a(n) == 1 (mod 4).
for(n=0, 100, print1(a(n) % 4 - 1, ", "));
