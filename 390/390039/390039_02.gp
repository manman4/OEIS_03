\\ a(n) = Sum_{k=0..floor(4*n/7)} binomial(k+3,4*n-7*k).
b(n) = sum(k=0, (4*n)\7, binomial(k+3, 4*n-7*k));
for(n=0, 43, print1(b(n), ", "));

\\ a(n) = 4*a(n-2) - 6*a(n-4) + 4*a(n-6) + a(n-7) - a(n-8).
a(n) = if(n<8, b(n), 4*a(n-2) - 6*a(n-4) + 4*a(n-6) + a(n-7) - a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

