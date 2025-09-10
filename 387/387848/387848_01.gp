\\ a(n) = Sum_{k=0..n} binomial(4*n,5*k).
b(n) = sum(k=0, n, binomial(4*n, 5*k));
for(n=0, 19, print1(b(n),", "));

\\ a(n) = 5*a(n-1) + 130*a(n-2) + 740*a(n-3) - 65*a(n-4) + 16*a(n-5) for n > 4.
a(n) = if(n<5, b(n), 5*a(n-1) + 130*a(n-2) + 740*a(n-3) - 65*a(n-4) + 16*a(n-5));
for(n=0, 25, print1(a(n)-b(n),", "));
