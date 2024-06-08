M=23;

\\ a(n) = n! * Sum_{k=0..floor(n/4)} binomial(3*n/4-3*k,k)/(n-4*k)!.
a(n) = n! * sum(k=0, n\4, binomial(3*n/4-3*k,k)/(n-4*k)!);
for(n=0, M, print1(a(n), ", "))