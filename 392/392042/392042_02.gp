\\ a(n) = Sum_{k=0..floor(n/3)} 2^k * 3^(n-3*k) * binomial(k,3*(n-3*k)).
b(n) = sum(k=0, n\3, 2^k * 3^(n-3*k) * binomial(k, 3*(n-3*k)));
for(n=0, 43, print1(b(n), ", "));

\\ a(n) = 6*a(n-3) - 12*a(n-6) + 8*a(n-9) + 24*a(n-10).
a(n) = if(n<10, b(n), 6*a(n-3) - 12*a(n-6) + 8*a(n-9) + 24*a(n-10));
for(n=0, 20, print1(a(n)-b(n), ", "));

