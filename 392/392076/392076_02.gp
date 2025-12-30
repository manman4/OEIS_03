\\ a(n) = Sum_{k=0..n} (k+1) * binomial(k,4*(n-k)).
b(n) = sum(k=0, n, (k+1) * binomial(k,4*(n-k)));
for(n=0, 33, print1(b(n), ", "));

\\ a(n) = 8*a(n-1) - 28*a(n-2) + 56*a(n-3) - 70*a(n-4) + 58*a(n-5) - 36*a(n-6) + 20*a(n-7) - 9*a(n-8) + 2*a(n-9) - a(n-10).
a(n) = if(n<10, b(n), 8*a(n-1) - 28*a(n-2) + 56*a(n-3) - 70*a(n-4) + 58*a(n-5) - 36*a(n-6) + 20*a(n-7) - 9*a(n-8) + 2*a(n-9) - a(n-10));
for(n=0, 20, print1(a(n)-b(n), ", "));

