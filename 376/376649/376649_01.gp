\\ a(n) = Sum_{k=0..floor(n/3)} binomial(floor(k/3),n-3*k).
a(n) = sum(k=0, n\3, binomial(k\3, n-3*k));
for(n=0, 85, print1(a(n),", "))

\\ a017877(n) = Sum_{k=0..floor(n/9)} binomial(k,n-9*k).
a017877(n) = sum(k=0, n\9, binomial(k, n-9*k));
\\ b(n) = A017877(n) + A017877(n-3) + A017877(n-6).
b(n) = a017877(n) + a017877(n-3) + a017877(n-6);
for(n=0, 200, print1(a(n)-b(n),", "))
