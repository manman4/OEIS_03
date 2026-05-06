\\ a(n) = Sum_{i,j,k,l >= 0 and i+j+k+l=n} binomial(4*i+2,i) * binomial(4*j+2,j) * binomial(4*k+2,k) * binomial(4*l+2,l).
a(n) = sum(i=0, n, sum(j=0, n-i, sum(k=0, n-i-j, binomial(4*i+2,i) * binomial(4*j+2,j) * binomial(4*k+2,k) * binomial(4*(n-i-j-k)+2,n-i-j-k))));
for(n=0, 10, print1(a(n), ", "));