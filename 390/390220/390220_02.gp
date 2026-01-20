\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k,4*n-8*k).
b(n) = sum(k=0, n\2, binomial(k, 4*n-8*k));
for(n=0, 51, print1(b(n), ", "));

\\ a(n) = 4*a(n-2) - 6*a(n-4) + 4*a(n-6) - a(n-8) + a(n-9).
a(n) = if(n<9, b(n), 4*a(n-2) - 6*a(n-4) + 4*a(n-6) - a(n-8) + a(n-9));
for(n=0, 20, print1(a(n)-b(n), ", "));

\\ a(n) = Sum_{k=0..n} binomial(k, 8*(n-k)).
a306752(n) = sum(k=0, n, binomial(k, 8*(n-k)));
\\ a(2*n) = A306752(n).
for(n=0, 100, print1(a306752(n) - b(2*n), ", "));

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(k+1,4*n-8*k+1).
a390221(n) = sum(k=0, n\2, binomial(k+1, 4*n-8*k+1));
\\ a(n) = A390221(n) - A390221(n-2).
for(n=0, 100, print1(a390221(n) - a390221(n-2) - b(n), ", "));