\\ a(n) = Sum_{k=0..n} (k+1) * binomial(n+3*k+7,n-k).
b(n) = sum(k=0, n, (k+1) * binomial(n+3*k+7, n-k));
for(n=0, 24, print1(b(n), ", "));

\\ a(n) = 10*a(n-1) - 37*a(n-2) + 68*a(n-3) - 78*a(n-4) + 58*a(n-5) - 28*a(n-6) + 8*a(n-7) - a(n-8).
a(n) = if(n<8, b(n), 10*a(n-1) - 37*a(n-2) + 68*a(n-3) - 78*a(n-4) + 58*a(n-5) - 28*a(n-6) + 8*a(n-7) - a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

