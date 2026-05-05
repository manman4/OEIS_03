\\ a(n) = Sum_{i,j,k,l,m >= 0 and i+j+k+l+m=n} binomial(5*i,i) * binomial(5*j,j) * binomial(5*k,k) * binomial(5*l,l) * binomial(5*m,m).
a(n) = sum(i=0, n, sum(j=0, n-i, sum(k=0, n-i-j, sum(l=0, n-i-j-k, binomial(5*i,i) * binomial(5*j,j) * binomial(5*k,k) * binomial(5*l,l) * binomial(5*(n-i-j-k-l),n-i-j-k-l)))));
for(n=0, 10, print1(a(n), ", "));