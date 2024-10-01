\\ a(n) = Sum_{k=0..floor(n/4)} binomial(floor(k/2),n-4*k).
a(n) = sum(k=0, n\4, binomial(k\2, n-4*k));
for(n=0, 77, print1(a(n),", "))

\\ a017867(n) = Sum_{k=0..floor(n/8)} binomial(k,n-8*k).
a017867(n) = sum(k=0, n\8, binomial(k, n-8*k));
\\ b(n) = A017867(n) + A017867(n-4).
b(n) = a017867(n) + a017867(n-4);
for(n=0, 200, print1(a(n)-b(n),", "))
