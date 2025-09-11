\\ a(n) = Sum_{k=0..n} binomial(2*n+1,6*k).
b(n) = sum(k=0, n, binomial(2*n+1, 6*k));
for(n=0, 26, print1(b(n),", "));

\\ a(n) = 6*a(n-1) - 15*a(n-2) + 22*a(n-3) + 15*a(n-4) + 36*a(n-5) for n > 4.
a(n) = if(n<5, b(n), 6*a(n-1) - 15*a(n-2) + 22*a(n-3) + 15*a(n-4) + 36*a(n-5));
for(n=0, 25, print1(a(n)-b(n),", "));


