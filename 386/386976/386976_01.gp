\\ a(n) = Sum_{k=0..n} binomial(3*n+2,4*k).
b(n) = sum(k=0, n, binomial(3*n+2, 4*k));
for(n=0, 20, print1(b(n),", "));

\\ a(n) = 4*a(n-1) + 24*a(n-2) + 64*a(n-3) for n > 2.
a(n) = if(n<3, b(n), 4*a(n-1) + 24*a(n-2) + 64*a(n-3));
for(n=0, 25, print1(a(n)-b(n),", "));


