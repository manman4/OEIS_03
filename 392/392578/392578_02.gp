\\ a(n) = Sum_{k=0..n} (k+1) * binomial(n+2*k+5,n-k).
b(n) = sum(k=0, n, (k+1) * binomial(n+2*k+5, n-k));
for(n=0, 26, print1(b(n), ", "));

\\ a(n) = 8*a(n-1) - 22*a(n-2) + 26*a(n-3) - 17*a(n-4) + 6*a(n-5) - a(n-6).
a(n) = if(n<6, b(n), 8*a(n-1) - 22*a(n-2) + 26*a(n-3) - 17*a(n-4) + 6*a(n-5) - a(n-6));
for(n=0, 20, print1(a(n)-b(n), ", "));

