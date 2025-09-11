\\ a(n) = Sum_{k=0..n} binomial(3*n+1,6*k).
b(n) = sum(k=0, n, binomial(3*n+1, 6*k));
for(n=0, 21, print1(b(n),", "));

\\ a(n) = 7*a(n-1) - 19*a(n-2) + 189*a(n-3) + 216*a(n-4) for n > 3.
a(n) = if(n<4, b(n), 7*a(n-1) - 19*a(n-2) + 189*a(n-3) + 216*a(n-4));
for(n=0, 25, print1(a(n)-b(n),", "));


