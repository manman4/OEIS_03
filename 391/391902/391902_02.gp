M=25;

\\ a(n) = Sum_{k=0..n} binomial(3*k,3*(n-k)).
b(n) = sum(k=0, n, binomial(3*k, 3*(n-k)));
for(n=0, M, print1(b(n),", "));

\\ a(n) = Sum_{k=0..floor(n/2)} binomial(3*(n-k),3*k).
c(n) = sum(k=0, n\2, binomial(3*(n-k), 3*k));
for(n=0, M, print1(b(n)-c(n),", "));

\\ a(n) = 3*a(n-1) + 22*a(n-3) + 3*a(n-5) + a(n-6).
a(n) = if(n<6, b(n), 3*a(n-1) + 22*a(n-3) + 3*a(n-5) + a(n-6));
for(n=0, 20, print1(a(n)-b(n),", "));