\\ a(n) = Sum_{k=0..floor(n/3)} 2^k * 3^(n-3*k) * binomial(k,4*(n-3*k)).
b(n) = sum(k=0, n\3, 2^k * 3^(n-3*k) * binomial(k,4*(n-3*k)));
for(n=0, 47, print1(b(n), ", "));

\\ a(n) = 8*a(n-3) - 24*a(n-6) + 32*a(n-9) - 16*a(n-12) + 48*a(n-13).
a(n) = if(n<13, b(n), 8*a(n-3) - 24*a(n-6) + 32*a(n-9) - 16*a(n-12) + 48*a(n-13));
for(n=0, 25, print1(a(n)-b(n), ", "));

