\\ a(n) = Sum_{k=0..floor(2*n/9)} binomial(2*n-7*k,2*k).
b(n) = sum(k=0, (2*n)\9, binomial(2*n-7*k, 2*k));
for(n=0, 37, print1(b(n), ", "));

\\ a(n) = 2*a(n-1) - a(n-2) + 4*a(n-5) + a(n-9).
a(n) = if(n<9, b(n), 2*a(n-1) - a(n-2) + 4*a(n-5) + a(n-9));
for(n=0, 20, print1(a(n)-b(n), ", "));

