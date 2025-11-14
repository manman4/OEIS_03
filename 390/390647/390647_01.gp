M=19;

\\ a(n) = n! * Sum_{k=0..floor(n/2)} k^(n-2*k) * A001764(k)/(n-2*k)!.
a001764(n) = binomial(3*n, n)/(2*n+1);
a(n) = n!*sum(k=0, n\2, k^(n-2*k) * a001764(k)/(n-2*k)!);
for(n=0, M, print1(a(n),", "));

b(n) = n!*sum(k=0, n\2, k^(n-2*k)*binomial(3*k, k)/((2*k+1)*(n-2*k)!));
for(n=0, 100, print1(a(n)-b(n),", "));


