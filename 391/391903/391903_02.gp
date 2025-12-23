M=34;

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(3*k,3*(n-2*k)).
b(n) = sum(k=0, n\2, binomial(3*k, 3*(n-2*k)));
for(n=0, M, print1(b(n),", "));

\\ a(n) = 3*a(n-2) + 3*a(n-3) - 3*a(n-4) + 21*a(n-5) - 2*a(n-6) + 3*a(n-7) + 3*a(n-8) + a(n-9).
a(n) = if(n<9, b(n), 3*a(n-2) + 3*a(n-3) - 3*a(n-4) + 21*a(n-5) - 2*a(n-6) + 3*a(n-7) + 3*a(n-8) + a(n-9));
for(n=0, 20, print1(a(n)-b(n),", "));