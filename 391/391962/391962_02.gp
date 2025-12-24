\\ a(n) = Sum_{k=0..n} 2^k * binomial(k+2,2) * binomial(k,2*(n-k)).
b(n) = sum(k=0, n, 2^k * binomial(k+2,2) * binomial(k,2*(n-k)));
for(n=0, 25, print1(b(n), ", "));

\\ a(n) = 12*a(n-1) - 60*a(n-2) + 172*a(n-3) - 336*a(n-4) + 480*a(n-5) - 496*a(n-6) + 384*a(n-7) - 192*a(n-8) + 64*a(n-9).
a(n) = if(n<9, b(n), 12*a(n-1) - 60*a(n-2) + 172*a(n-3) - 336*a(n-4) + 480*a(n-5) - 496*a(n-6) + 384*a(n-7) - 192*a(n-8) + 64*a(n-9));
for(n=0, 20, print1(a(n)-b(n), ", "));

