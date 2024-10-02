\\ a(n) = Sum_{k=0..floor(n/2)} binomial(n-2*k,floor(k/3)).
a(n) = sum(k=0, n\2, binomial(n-2*k, k\3));
for(n=0, 54, print1(a(n),", "))

\\ a005709(n) = Sum_{k=0..floor(n/6)} binomial(n-6*k,k).
a005709(n) = sum(k=0, n\6, binomial(n-6*k, k));
for(n=0, 54, print1(a005709(n),", "))
\\ b(n) = A005709(n) + A005709(n-2) + A005709(n-4).
b(n) = a005709(n) + a005709(n-2) + a005709(n-4);
for(n=0, 200, print1(a(n)-b(n),", "))


