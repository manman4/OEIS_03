M=500;

\\ a(n) = Sum_{i=0..n} Sum_{j=0..i} binomial(n+3*i,4*i) * binomial(i+3*j,4*j).
a(n) = sum(i=0, n, sum(j=0, i, binomial(n+3*i, 4*i) * binomial(i+3*j, 4*j)));
for(n=0, M, write("b396208_1.txt", n, " ", a(n)));