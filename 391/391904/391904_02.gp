M=39;

\\ a(n) = Sum_{k=0..floor(n/3)} binomial(3*k,3*(n-3*k)).
b(n) = sum(k=0, n\3, binomial(3*k, 3*(n-3*k)));
for(n=0, M, print1(b(n),", "));

\\ a(n) = 3*a(n-3) + 3*a(n-4) - 3*a(n-6) + 21*a(n-7) - 3*a(n-8) + a(n-9) + 3*a(n-10) + 3*a(n-11) + a(n-12).
a(n) = if(n<12, b(n), 3*a(n-3) + 3*a(n-4) - 3*a(n-6) + 21*a(n-7) - 3*a(n-8) + a(n-9) + 3*a(n-10) + 3*a(n-11) + a(n-12));
for(n=0, 20, print1(a(n)-b(n),", "));