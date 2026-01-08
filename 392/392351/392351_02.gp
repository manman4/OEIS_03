\\ a(n) = Sum_{k=0..floor(3*n/14)} binomial(3*n-13*k,k).
b(n) = sum(k=0, (3*n)\14, binomial(3*n-13*k, k));
for(n=0, 42, print1(b(n), ", "));

\\ a(n) = a(n-1) + 3*a(n-5) + a(n-14).
a(n) = if(n<14, b(n), a(n-1) + 3*a(n-5) + a(n-14));
for(n=0, 25, print1(a(n)-b(n), ", "));

