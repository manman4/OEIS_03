\\ a(n) = Sum_{k=0..floor(n/4)} binomial(2*n-5*k,3*k).
b(n) = sum(k=0, n\4, binomial(2*n-5*k, 3*k));
for(n=0, 33, print1(b(n), ", "));

\\ a(n) = 3*a(n-1) - 3*a(n-2) + a(n-3) + 2*a(n-4) + 6*a(n-5) - a(n-8).
a(n) = if(n<8, b(n), 3*a(n-1) - 3*a(n-2) + a(n-3) + 2*a(n-4) + 6*a(n-5) - a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

