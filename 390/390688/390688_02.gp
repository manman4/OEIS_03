\\ a(n) = Sum_{k=0..floor(2*n/9)} binomial(k+1,2*n-9*k).
b(n) = sum(k=0, (2*n)\9, binomial(k+1, 2*n-9*k));
for(n=0, 67, print1(b(n), ", "));

\\ a(n) = 2*a(n-5) + a(n-9) - a(n-10).
a(n) = if(n<10, b(n), 2*a(n-5) + a(n-9) - a(n-10));
for(n=0, 20, print1(a(n)-b(n), ", "));

