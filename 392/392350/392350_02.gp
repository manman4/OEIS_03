\\ a(n) = Sum_{k=0..floor(3*n/11)} binomial(3*n-10*k,k).
b(n) = sum(k=0, (3*n)\11, binomial(3*n-10*k, k));
for(n=0, 39, print1(b(n), ", "));

\\ a(n) = 2*a(n-1) - 2*a(n-2) + 2*a(n-3) + a(n-4) - a(n-5) + a(n-6) - a(n-7) + a(n-8) - a(n-9) + a(n-10).
a(n) = if(n<10, b(n), 2*a(n-1) - 2*a(n-2) + 2*a(n-3) + a(n-4) - a(n-5) + a(n-6) - a(n-7) + a(n-8) - a(n-9) + a(n-10));
for(n=0, 25, print1(a(n)-b(n), ", "));

