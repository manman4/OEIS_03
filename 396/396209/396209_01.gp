M=500;

\\ a(n) = Sum_{i=1..n} Sum_{j=1..i} binomial(j+1,2) * binomial(n+2*i-1,3*i-1) * binomial(i+2*j-1,3*j-1).
a(n) = sum(i=1, n, sum(j=1, i, binomial(j+1, 2) * binomial(n+2*i-1, 3*i-1) * binomial(i+2*j-1, 3*j-1)));
for(n=0, M, write("b396209_1.txt", n, " ", a(n)));