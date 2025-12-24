\\ a(n) = Sum_{k=0..floor(n/3)} 2^k * 3^(n-3*k) * binomial(k,2*(n-3*k)).
b(n) = sum(k=0, n\3, 2^k * 3^(n-3*k) * binomial(k, 2*(n-3*k)));
for(n=0, 40, print1(b(n), ", "));

\\ a(n) = 4*a(n-3) - 4*a(n-6) + 12*a(n-7).
a(n) = if(n<7, b(n), 4*a(n-3) - 4*a(n-6) + 12*a(n-7));
for(n=0, 20, print1(a(n)-b(n), ", "));

