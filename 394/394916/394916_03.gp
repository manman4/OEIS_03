\\ a(n) = Sum_{i,j,k >= 0 and i+j+k=n} binomial(5*i,i) * binomial(5*j,j) * binomial(5*k,k).
a(n) = sum(i=0, n, sum(j=0, n-i, binomial(5*i,i) * binomial(5*j,j) * binomial(5*(n-i-j),n-i-j)));
for(n=0, 10, print1(a(n), ", "));