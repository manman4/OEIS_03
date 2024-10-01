\\ a(n) = Sum_{k=0..floor(n/3)} binomial(floor(k/2),n-3*k).
a(n) = sum(k=0, n\3, binomial(k\2, n-3*k));
for(n=0, 71, print1(a(n),", "))

\\ a017847(n) = Sum_{k=0..floor(n/6)} binomial(k,n-6*k).
a017847(n) = sum(k=0, n\6, binomial(k, n-6*k));
\\ b(n) = A017847(n) + A017847(n-3).
b(n) = a017847(n) + a017847(n-3);
for(n=0, 200, print1(a(n)-b(n),", "))
