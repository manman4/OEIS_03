M=27;

\\ a(n) = Sum_{k=0..n} binomial(k+2,2) * binomial(3*k,n-k).
b(n) = sum(k=0, n, binomial(k+2,2)*binomial(3*k,n-k));
for(n=0, M, print1(b(n),", "));

\\ a(n) = 3*a(n-1) + 6*a(n-2) - 8*a(n-3) - 33*a(n-4) - 24*a(n-5) + 39*a(n-6) + 108*a(n-7) + 123*a(n-8) + 84*a(n-9) + 36*a(n-10) + 9*a(n-11) + a(n-12).
a(n) = if(n<12, b(n), 3*a(n-1) + 6*a(n-2) - 8*a(n-3) - 33*a(n-4) - 24*a(n-5) + 39*a(n-6) + 108*a(n-7) + 123*a(n-8) + 84*a(n-9) + 36*a(n-10) + 9*a(n-11) + a(n-12));
for(n=0, 20, print1(a(n)-b(n),", "));


