M=21;

\\ a(n) = n! * Sum_{k=0..floor(n/3)} k^(n-3*k) * Catalan(k)/(n-3*k)!.
catalan(n) = binomial(2*n, n)/(n+1);
a(n) = n!*sum(k=0, n\3, k^(n-3*k) * catalan(k)/(n-3*k)!);
for(n=0, M, print1(a(n),", "));

b(n) = n!*sum(k=0, n\3, k^(n-3*k)*binomial(2*k, k)/((k+1)*(n-3*k)!));
for(n=0, 100, print1(a(n)-b(n),", "));


