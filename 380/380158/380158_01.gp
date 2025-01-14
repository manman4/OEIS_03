M=22;

\\ a(n) = n! * Sum_{k=0..n} 2^k * (2*k-1)^(n-k) * binomial(1/2,k)/(n-k)!.
a(n) = n! * sum(k=0, n, 2^k * (2*k-1)^(n-k) * binomial(1/2,k)/(n-k)!);
for(n=0, M, print1(a(n),", "));

\\ a(n) == 0 (mod 2) for n>0.
for(n=0, 200, print1(a(n) % 2, ", "));

