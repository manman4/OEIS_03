M=20;

\\ a(n) = n! * Sum_{k=0..floor(n/3)} 9^k * binomial(n/3-1,k)/(n-3*k)!.
a(n) = n! * sum(k=0, n\3, 9^k * binomial(n/3-1,k)/(n-3*k)!);
for(n=0, M, print1(a(n)", "));

\\ a(n) == 1 mod 72.
for(n=0, 200, print1(a(n)%72", "));