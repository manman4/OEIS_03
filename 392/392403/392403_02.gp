\\ a(n) = Sum_{k=0..floor(2*n/7)} binomial(2*n-4*k,3*k).
b(n) = sum(k=0, (2*n)\7, binomial(2*n-4*k, 3*k));
for(n=0, 31, print1(b(n), ", "));

\\ a(n) = 3*a(n-1) - 3*a(n-2) + a(n-3) + 6*a(n-4) + 2*a(n-5) + a(n-7).
a(n) = if(n<7, b(n), 3*a(n-1) - 3*a(n-2) + a(n-3) + 6*a(n-4) + 2*a(n-5) + a(n-7));
for(n=0, 20, print1(a(n)-b(n), ", "));

