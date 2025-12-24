\\ a(n) = Sum_{k=0..n} binomial(k+2,2) * binomial(k,2*(n-k)).
b(n) = sum(k=0, n, binomial(k+2,2) * binomial(k,2*(n-k)));
for(n=0, 33, print1(b(n), ", "));

\\ a(n) = 6*a(n-1) - 15*a(n-2) + 23*a(n-3) - 27*a(n-4) + 24*a(n-5) - 16*a(n-6) + 9*a(n-7) - 3*a(n-8) + a(n-9).
a(n) = if(n<9, b(n), 6*a(n-1) - 15*a(n-2) + 23*a(n-3) - 27*a(n-4) + 24*a(n-5) - 16*a(n-6) + 9*a(n-7) - 3*a(n-8) + a(n-9));
for(n=0, 20, print1(a(n)-b(n), ", "));

