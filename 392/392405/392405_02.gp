\\ a(n) = Sum_{k=0..floor(2*n/9)} binomial(2*n-6*k,3*k).
b(n) = sum(k=0, (2*n)\9, binomial(2*n-6*k, 3*k));
for(n=0, 34, print1(b(n), ", "));

\\ a(n) = 3*a(n-1) - 3*a(n-2) + a(n-3) + 6*a(n-5) + 2*a(n-6) + a(n-9).
a(n) = if(n<9, b(n), 3*a(n-1) - 3*a(n-2) + a(n-3) + 6*a(n-5) + 2*a(n-6) + a(n-9));
for(n=0, 20, print1(a(n)-b(n), ", "));

