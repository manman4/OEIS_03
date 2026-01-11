\\ a(n) = Sum_{k=0..2*n} binomial(3*k,2*n-k).
b(n) = sum(k=0, 2*n, binomial(3*k, 2*n-k));
for(n=0, 22, print1(b(n), ", "));

\\ a(n) = 7*a(n-1) - a(n-2) + 3*a(n-3) - a(n-4).
a(n) = if(n<4, b(n), 7*a(n-1) - a(n-2) + 3*a(n-3) - a(n-4));
for(n=0, 20, print1(a(n)-b(n), ", "));

c(n) = polcoeff( if( n<0, (1 + x^3) / (1 + x^3 - x^4), 1 / (1 - x - x^4)) + x * O(x^abs(n)), abs(n));
a003269(n) = c(n-1);
for(n=0, 20, print1(a003269(n), ", "));
\\ a(n) = A003269(6*n+1).
for(n=0, 100, print1(b(n)-a003269(6*n+1), ", "));
