\\ a(n) = Sum_{i,j,k,l,m >= 0 and i+j+k+l+m=n} binomial(4*i,i) * binomial(4*j,j) * binomial(4*k,k) * binomial(4*l,l) * binomial(4*m,m).
a(n) = sum(i=0, n, sum(j=0, n-i, sum(k=0, n-i-j, sum(l=0, n-i-j-k, binomial(4*i,i) * binomial(4*j,j) * binomial(4*k,k) * binomial(4*l,l) * binomial(4*(n-i-j-k-l),n-i-j-k-l)))));
for(n=0, 10, print1(a(n), ", "));