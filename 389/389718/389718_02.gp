\\ a(n) = Sum_{k=0..floor(5*n/14)} binomial(k,5*n-14*k).
b(n) = sum(k=0, (5*n)\14, binomial(k, 5*n-14*k));
for(n=0, 62, print1(b(n), ", "));

\\ a(n) = 5*a(n-3) - 10*a(n-6) + 10*a(n-9) - 5*a(n-12) + a(n-14) + a(n-15).
a(n) = if(n<15, b(n), 5*a(n-3) - 10*a(n-6) + 10*a(n-9) - 5*a(n-12) + a(n-14) + a(n-15));
for(n=0, 25, print1(a(n)-b(n), ", "));

