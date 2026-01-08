\\ a(n) = Sum_{k=0..floor(3*n/8)} binomial(3*n-7*k,k).
b(n) = sum(k=0, (3*n)\8, binomial(3*n-7*k, k));
for(n=0, 36, print1(b(n), ", "));

\\ a(n) = a(n-1) + 3*a(n-3) + a(n-8).
a(n) = if(n<8, b(n), a(n-1) + 3*a(n-3) + a(n-8));
for(n=0, 20, print1(a(n)-b(n), ", "));

