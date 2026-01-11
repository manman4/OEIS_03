\\ a(n) = Sum_{k=0..floor(2*n/5)} binomial(3*k,2*n-5*k).
b(n) = sum(k=0, (2*n)\5, binomial(3*k, 2*n-5*k));
for(n=0, 36, print1(b(n), ", "));

\\ a(n) = 6*a(n-3) + 2*a(n-4) + a(n-5) - 3*a(n-6) + 3*a(n-7) - a(n-8).
a(n) = if(n<8, b(n), 6*a(n-3) + 2*a(n-4) + a(n-5) - 3*a(n-6) + 3*a(n-7) - a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

