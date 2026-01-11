\\ a(n) = Sum_{k=0..floor(2*n/7)} binomial(2*k,2*n-7*k).
b(n) = sum(k=0, (2*n)\7, binomial(2*k, 2*n-7*k));
for(n=0, 49, print1(b(n), ", "));

\\ a(n) = 4*a(n-4) + a(n-7) - 2*a(n-8) + a(n-9).
a(n) = if(n<9, b(n), 4*a(n-4) + a(n-7) - 2*a(n-8) + a(n-9));
for(n=0, 20, print1(a(n)-b(n), ", "));

