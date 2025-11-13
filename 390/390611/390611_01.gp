M=18;

\\ a(n) = n! * Sum_{k=0..floor(n/3)} (n-3*k)^k * A001764(n-3*k)/k!.
a001764(n) = binomial(3*n, n)/(2*n+1);
a(n) = n!*sum(k=0, n\3, (n-3*k)^k * a001764(n-3*k)/k!);
for(n=0, M, print1(a(n),", "));

b(n) = n!*sum(k=0, n\3, (n-3*k)^k*binomial(3*(n-3*k), n-3*k)/((2*(n-3*k)+1)*k!));
for(n=0, 100, print1(a(n)-b(n),", "));


