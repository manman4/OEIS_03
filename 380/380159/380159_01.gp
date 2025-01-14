M=20;

\\ a(n) = n! * Sum_{k=0..n} 3^k * (3*k-1)^(n-k) * binomial(1/3,k)/(n-k)!.
a(n) = n! * sum(k=0, n, 3^k * (3*k-1)^(n-k) * binomial(1/3,k)/(n-k)!);
for(n=0, M, print1(a(n),", "));

\\ a(n) == 0 (mod 3) for n>0.
for(n=0, 200, print1(a(n) % 3, ", "));

