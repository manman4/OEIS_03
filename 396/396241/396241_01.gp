M=200;

\\ a(0) = 1; a(n) = Sum_{i=1..n} Sum_{j=1..i} Sum_{k=1..j} binomial(n+i-1,2*i-1) * binomial(i+j-1,2*j-1) * binomial(j+k-1,2*k-1).
a(n) = if(n==0, 1, sum(i=1, n, sum(j=1, i, sum(k=1, j, binomial(n+i-1, 2*i-1) * binomial(i+j-1, 2*j-1) * binomial(j+k-1, 2*k-1)))));
for(n=0, M, write("b396241_1.txt", n, " ", a(n)));