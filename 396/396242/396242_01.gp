M=200;

\\ a(0) = 1; a(n) = Sum_{i=1..n} Sum_{j=1..i} binomial(n+2*i-1,3*i-1) * binomial(i+2*j-1,3*j-1).
a(n) = if(n==0, 1, sum(i=1, n, sum(j=1, i, binomial(n+2*i-1, 3*i-1) * binomial(i+2*j-1, 3*j-1))));
for(n=0, M, write("b396242_1.txt", n, " ", a(n)));