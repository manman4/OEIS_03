\\ a(n) = Sum_{k=0..floor(2*n/5)} binomial(2*n-3*k,2*k).
b(n) = sum(k=0, (2*n)\5, binomial(2*n-3*k, 2*k));
for(n=0, 32, print1(b(n), ", "));

\\ a(n) = 2*a(n-1) - a(n-2) + 4*a(n-3) + a(n-5).
a(n) = if(n<5, b(n), 2*a(n-1) - a(n-2) + 4*a(n-3) + a(n-5));
for(n=0, 20, print1(a(n)-b(n), ", "));

