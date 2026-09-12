\\ a(n) = Sum_{i,j,k,l >= 0 and i+j+k+l=n} binomial(5*i+1,i) * binomial(5*j+1,j) * binomial(5*k+1,k) * binomial(5*l+1,l).
a(n) = sum(i=0, n, sum(j=0, n-i, sum(k=0, n-i-j, binomial(5*i+1,i) * binomial(5*j+1,j) * binomial(5*k+1,k) * binomial(5*(n-i-j-k)+1,n-i-j-k))));
for(n=0, 10, print1(a(n), ", "));