M=16;

a000108(n) = binomial(2*n,n)/(n+1);
a001764(n) = binomial(3*n,n)/(2*n+1);
a002293(n) = binomial(4*n,n)/(3*n+1);
a002294(n) = binomial(5*n,n)/(4*n+1);
a002295(n) = binomial(6*n,n)/(5*n+1);

\\ a(n) = n! * Sum_{k=0..n} (2*k+1)^(n-k) * A002294(k)/(n-k)!.
a(n) = n! * sum(k=0, n, (2*k+1)^(n-k) * a002294(k) / (n-k)!);
for(n=0, M, print1(a(n), ", "));


