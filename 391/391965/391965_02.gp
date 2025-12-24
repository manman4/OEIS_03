\\ a(n) = Sum_{k=0..n} (k+1) * 2^k * binomial(k,2*(n-k)).
b(n) = sum(k=0, n, (k+1) * 2^k * binomial(k,2*(n-k)));
for(n=0, 26, print1(b(n), ", "));

\\ a(n) = 8*a(n-1) - 24*a(n-2) + 40*a(n-3) - 48*a(n-4) + 32*a(n-5) - 16*a(n-6).
a(n) = if(n<6, b(n), 8*a(n-1) - 24*a(n-2) + 40*a(n-3) - 48*a(n-4) + 32*a(n-5) - 16*a(n-6));
for(n=0, 20, print1(a(n)-b(n), ", "));

