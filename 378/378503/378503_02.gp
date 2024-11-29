\\ a(n) = Sum_{i+j+k=n, i,j,k >= 0} binomial(4*i,i) * binomial(4*j,j) * binomial(4*k,k).
a(n) = sum(i=0, n, sum(j=0, n-i, binomial(4*i,i) * binomial(4*j,j) * binomial(4*(n-i-j),n-i-j)));
for(n=0, 20, print1(a(n), ", "));