\\ a(n) = Sum_{k=0..floor(5*n/9)} binomial(k,5*n-9*k).
b(n) = sum(k=0, (5*n)\9, binomial(k, 5*n-9*k));
for(n=0, 47, print1(b(n), ", "));

\\ a(n) = 5*a(n-2) - 10*a(n-4) + 10*a(n-6) - 5*a(n-8) + a(n-9) + a(n-10).
a(n) = if(n<10, b(n), 5*a(n-2) - 10*a(n-4) + 10*a(n-6) - 5*a(n-8) + a(n-9) + a(n-10));
for(n=0, 25, print1(a(n)-b(n), ", "));

