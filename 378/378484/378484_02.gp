\\ a(n) = Sum_{i+j+k+l=n, i,j,k,l >= 0} binomial(4*i,i) * binomial(4*j,j) * binomial(4*k,k) * binomial(4*l,l).
a(n) = sum(i=0, n, sum(j=0, n-i, sum(k=0, n-i-j, binomial(4*i,i) * binomial(4*j,j) * binomial(4*k,k) * binomial(4*(n-i-j-k),n-i-j-k))));
for(n=0, 20, print1(a(n), ", "));