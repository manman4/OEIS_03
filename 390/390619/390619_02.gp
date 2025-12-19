\\ a(n) = Sum_{k=0..floor(n/3)} 2^k * 3^(n-3*k) * binomial(2*k,2*(n-3*k)).
b(n) = sum(k=0, n\3, 2^k * 3^(n-3*k) * binomial(2*k,2*(n-3*k)));
for(n=0, 35, print1(b(n), ", "));

\\ a(n) = 4*a(n-3) + 12*a(n-4) - 4*a(n-6) + 24*a(n-7) - 36*a(n-8).
a(n) = if(n<8, b(n), 4*a(n-3) + 12*a(n-4) - 4*a(n-6) + 24*a(n-7) - 36*a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

