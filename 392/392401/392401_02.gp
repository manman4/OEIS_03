\\ a(n) = Sum_{k=0..floor(2*n/5)} binomial(2*n-2*k,3*k).
b(n) = sum(k=0, (2*n)\5, binomial(2*n-2*k, 3*k));
for(n=0, 29, print1(b(n), ", "));

\\ a(n) = 3*a(n-1) - 3*a(n-2) + 7*a(n-3) + 2*a(n-4) + a(n-5).
a(n) = if(n<5, b(n), 3*a(n-1) - 3*a(n-2) + 7*a(n-3) + 2*a(n-4) + a(n-5));
for(n=0, 20, print1(a(n)-b(n), ", "));

