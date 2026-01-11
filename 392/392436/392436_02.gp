\\ a(n) = Sum_{k=0..floor(2*n/7)} binomial(3*k,2*n-7*k).
b(n) = sum(k=0, (2*n)\7, binomial(3*k, 2*n-7*k));
for(n=0, 42, print1(b(n), ", "));

\\ a(n) = 6*a(n-4) + 2*a(n-5) + a(n-7) - 3*a(n-8) + 3*a(n-9) - a(n-10).
a(n) = if(n<10, b(n), 6*a(n-4) + 2*a(n-5) + a(n-7) - 3*a(n-8) + 3*a(n-9) - a(n-10));
for(n=0, 20, print1(a(n)-b(n), ", "));

