\\ a(n) = Sum_{k=0..floor(3*n/8)} binomial(k,3*n-8*k).
b(n) = sum(k=0, (3*n)\8, binomial(k, 3*n-8*k));
for(n=0, 55, print1(b(n), ", "));

\\ a(n) = 3*a(n-3) - 3*a(n-6) + a(n-8) + a(n-9).
a(n) = if(n<9, b(n), 3*a(n-3) - 3*a(n-6) + a(n-8) + a(n-9));
for(n=0, 25, print1(a(n)-b(n), ", "));

