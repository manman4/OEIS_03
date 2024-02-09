M=16;

a(n) = 4*(4*n+3)!/(3*n+4)!;
for(n=0, M, print1(a(n), ", "))

a002293(n) = binomial(4*n, n)/(3*n + 1);
for(n=0, M, print1(a002293(n), ", "))
\\ a(n) = A000142(n) * A002293(n+1).
for(n=0, M, print1(a(n) - n! * a002293(n+1), ", "))