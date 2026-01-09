\\ a(n) = Sum_{k=0..floor(4*n/11)} binomial(k,4*n-11*k).
b(n) = sum(k=0, (4*n)\11, binomial(k, 4*n-11*k));
for(n=0, 59, print1(b(n), ", "));

\\ a(n) = 4*a(n-3) - 6*a(n-6) + 4*a(n-9) + a(n-11) - a(n-12).
a(n) = if(n<12, b(n), 4*a(n-3) - 6*a(n-6) + 4*a(n-9) + a(n-11) - a(n-12));
for(n=0, 25, print1(a(n)-b(n), ", "));

