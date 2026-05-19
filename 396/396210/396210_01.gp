M=500;

\\ a(n) = Sum_{i=1..n} Sum_{j=1..i} binomial(j+2,3) * binomial(n+3*i-1,4*i-1) * binomial(i+3*j-1,4*j-1).
a(n) = sum(i=1, n, sum(j=1, i, binomial(j+2, 3) * binomial(n+3*i-1, 4*i-1) * binomial(i+3*j-1, 4*j-1)));
for(n=0, M, write("b396210_1.txt", n, " ", a(n)));