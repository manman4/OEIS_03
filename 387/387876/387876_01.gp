\\ a(n) = Sum_{k=0..n} binomial(4*n+1,6*k).
b(n) = sum(k=0, n, binomial(4*n+1, 6*k));
for(n=0, 18, print1(b(n),", "));

\\ a(n) = 6*a(n-1) + 69*a(n-2) + 1366*a(n-3) + 1359*a(n-4) + 1296*a(n-5) for n > 4.
a(n) = if(n<5, b(n), 6*a(n-1) + 69*a(n-2) + 1366*a(n-3) + 1359*a(n-4) + 1296*a(n-5));
for(n=0, 25, print1(a(n)-b(n),", "));


