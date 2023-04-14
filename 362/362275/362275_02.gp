M=21;

a(n) = sum(i=0, n, (-1)^(n-i) * binomial(n,i) * sum(j=0, n-i, i^j * j^(n-i-j) * binomial(n-i,j)));
for(n=0, M, print1(a(n), ", ")); 