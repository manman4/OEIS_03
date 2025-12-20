\\ a(n) = Sum_{k=0..n} (k+1) * 2^k * 3^(n-k) * binomial(2*k,2*(n-k)).
b(n) = sum(k=0, n, (k+1) * 2^k * 3^(n-k) * binomial(2*k,2*(n-k)));
for(n=0, 22, print1(b(n), ", "));

\\ a(n) = 8*a(n-1) - 16*a(n-3) - 328*a(n-4) - 96*a(n-5) + 1728*a(n-7) - 1296*a(n-8).
a(n) = if(n<8, b(n), 8*a(n-1) - 16*a(n-3) - 328*a(n-4) - 96*a(n-5) + 1728*a(n-7) - 1296*a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

