\\ a(n) = Sum_{i,j,k,l >= 0 and i+j+k+l=n} binomial(5*i,i) * binomial(5*j,j) * binomial(5*k,k) * binomial(5*l,l).
a(n) = sum(i=0, n, sum(j=0, n-i, sum(k=0, n-i-j, binomial(5*i,i) * binomial(5*j,j) * binomial(5*k,k) * binomial(5*(n-i-j-k),n-i-j-k))));
for(n=0, 10, print1(a(n), ", "));