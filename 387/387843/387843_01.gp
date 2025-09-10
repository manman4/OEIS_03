\\ a(n) = Sum_{k=0..n} binomial(2*n,3*k).
b(n) = sum(k=0, n, binomial(2*n, 3*k));
for(n=0, 26, print1(b(n),", "));

\\ a(n) = 3*a(n-1) + 3*a(n-2) + 4*a(n-3) for n > 2.
a(n) = if(n<3, b(n), 3*a(n-1) + 3*a(n-2) + 4*a(n-3));
for(n=0, 25, print1(a(n)-b(n),", "));
