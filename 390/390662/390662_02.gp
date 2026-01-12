\\ a(n) = Sum_{k=0..floor(2*n/7)} binomial(k+1,2*n-7*k).
b(n) = sum(k=0, (2*n)\7, binomial(k+1, 2*n-7*k));
for(n=0, 59, print1(b(n), ", "));

\\ a(n) = 2*a(n-4) + a(n-7) - a(n-8).
a(n) = if(n<8, b(n), 2*a(n-4) + a(n-7) - a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

