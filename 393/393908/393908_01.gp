M=500;

\\ a(n) = Sum_{i=0..n} Sum_{j=0..i} binomial(n+2*i,3*i) * binomial(i+2*j,3*j).
a(n) = sum(i=0, n, sum(j=0, i, binomial(n+2*i, 3*i) * binomial(i+2*j, 3*j)));
for(n=0, M, write("b393908_1.txt", n, " ", a(n)));