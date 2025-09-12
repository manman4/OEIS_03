\\ a(n) = Sum_{k=0..n} binomial(3*n+2,5*k).
b(n) = sum(k=0, n, binomial(3*n+2, 5*k));
for(n=0, 20, print1(b(n),", "));

\\ a(n) = 5*a(n-1) + 5*a(n-2) + 145*a(n-3) + 55*a(n-4) + 8*a(n-5) for n > 4.
a(n) = if(n<5, b(n), 5*a(n-1) + 5*a(n-2) + 145*a(n-3) + 55*a(n-4) + 8*a(n-5));
for(n=0, 25, print1(a(n)-b(n),", "));


