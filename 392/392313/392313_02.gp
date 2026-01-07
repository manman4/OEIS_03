\\ a(n) = Sum_{k=0..n} binomial(k+2,2) * binomial(k,3*n-3*k).
b(n) = sum(k=0, n, binomial(k+2, 2) * binomial(k, 3*n-3*k));
for(n=0, 32, print1(b(n), ", "));

\\ a(n) = 9*a(n-1) - 36*a(n-2) + 84*a(n-3) - 123*a(n-4) + 108*a(n-5) - 39*a(n-6) - 24*a(n-7) + 33*a(n-8) - 8*a(n-9) - 6*a(n-10) + 3*a(n-11) + a(n-12).
a(n) = if(n<12, b(n), 9*a(n-1) - 36*a(n-2) + 84*a(n-3) - 123*a(n-4) + 108*a(n-5) - 39*a(n-6) - 24*a(n-7) + 33*a(n-8) - 8*a(n-9) - 6*a(n-10) + 3*a(n-11) + a(n-12));
for(n=0, 20, print1(a(n)-b(n), ", "));

