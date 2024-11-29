\\ a(n) = Sum_{i+j+k+l=n, i,j,k,l >= 0} binomial(3*i,i) * binomial(3*j,j) * binomial(3*k,k) * binomial(3*l,l).
a(n) = sum(i=0, n, sum(j=0, n-i, sum(k=0, n-i-j, binomial(3*i,i) * binomial(3*j,j) * binomial(3*k,k) * binomial(3*(n-i-j-k),n-i-j-k))));
for(n=0, 20, print1(a(n), ", "));