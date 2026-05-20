M=200;

\\ a(n) = Sum_{i=0..n} Sum_{j=0..i} Sum_{k=0..j} binomial(n+i,2*i) * binomial(i+j,2*j) * binomial(j+k,2*k).
a(n) = sum(i=0, n, sum(j=0, i, sum(k=0, j, binomial(n+i, 2*i) * binomial(i+j, 2*j) * binomial(j+k, 2*k))));
for(n=0, M, write("b396238_1.txt", n, " ", a(n)));