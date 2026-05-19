M=50;

\\ a(n) = Sum_{i=1..n} Sum_{j=1..i} j * binomial(n+i-1,2*i-1) * binomial(i+j-1,2*j-1).
a(n) = sum(i=1, n, sum(j=1, i, j * binomial(n+i-1, 2*i-1) * binomial(i+j-1, 2*j-1)));
for(n=1, M, print1(a(n), ", "));