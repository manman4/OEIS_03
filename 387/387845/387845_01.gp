\\ a(n) = Sum_{k=0..n} binomial(2*n,7*k).
b(n) = sum(k=0, n, binomial(2*n, 7*k));
for(n=0, 26, print1(b(n),", "));

\\ a(n) = 7*a(n-1) - 21*a(n-2) + 35*a(n-3) - 21*a(n-4) + 91*a(n-5) + 35*a(n-6) + 4*a(n-7) for n > 6.
a(n) = if(n<7, b(n), 7*a(n-1) - 21*a(n-2) + 35*a(n-3) - 21*a(n-4) + 91*a(n-5) + 35*a(n-6) + 4*a(n-7));
for(n=0, 25, print1(a(n)-b(n),", "));
