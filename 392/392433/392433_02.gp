\\ a(n) = Sum_{k=0..floor(n/2)} binomial(3*k,2*n-4*k).
b(n) = sum(k=0, (2*n)\4, binomial(3*k, 2*n-4*k));
for(n=0, 34, print1(b(n), ", "));

\\ a(n) = 2*a(n-2) + 6*a(n-3) - a(n-4) + 3*a(n-5) - 3*a(n-6) + a(n-7).
a(n) = if(n<7, b(n), 2*a(n-2) + 6*a(n-3) - a(n-4) + 3*a(n-5) - 3*a(n-6) + a(n-7));
for(n=0, 20, print1(a(n)-b(n), ", "));

