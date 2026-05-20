M=200;

\\ a(0) = 1; a(n) = Sum_{i=1..n} Sum_{j=1..i} Sum_{k=1..j} binomial(n+2*i-1,3*i-1) * binomial(i+2*j-1,3*j-1) * binomial(j+2*k-1,3*k-1).
a(n) = if(n==0, 1, sum(i=1, n, sum(j=1, i, sum(k=1, j, binomial(n+2*i-1, 3*i-1) * binomial(i+2*j-1, 3*j-1) * binomial(j+2*k-1, 3*k-1)))));
for(n=0, M, write("b396243_1.txt", n, " ", a(n)));