\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k,3*(n-2*k)).
b(n) = sum(k=0, n\2, binomial(k, 3*(n-2*k)));
for(n=0, 49, print1(b(n), ", "));

\\ a(n) = 3*a(n-2) - 3*a(n-4) + a(n-6) + a(n-7).
a(n) = if(n<7, b(n), 3*a(n-2) - 3*a(n-4) + a(n-6) + a(n-7));
for(n=0, 20, print1(a(n)-b(n), ", "));

\\ a(n) = Sum_{k=0..n} binomial(k, 6*(n-k)).
a293169(n) = sum(k=0, n, binomial(k, 6*(n-k)));

\\ a(2*n) = A293169(n).
for(n=0, 20, print1(b(2*n)-a293169(n), ", "));