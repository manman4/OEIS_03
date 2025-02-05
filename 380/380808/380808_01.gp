M=17;

\\ a(n) = n! * Sum_{k=0..n} (n+k+2)^k * binomial(n,k)/(k+1)!.
a(n) = n! * sum(k=0, n, (n+k+2)^k * binomial(n, k)/(k+1)!);
for(n=0, M, print1(a(n),", "));

