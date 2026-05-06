\\ a(n) = Sum_{i,j,k >= 0 and i+j+k=n} binomial(4*i+2,i) * binomial(4*j+2,j) * binomial(4*k+2,k).
a(n) = sum(i=0, n, sum(j=0, n-i, binomial(4*i+2,i) * binomial(4*j+2,j) * binomial(4*(n-i-j)+2,n-i-j)));
for(n=0, 10, print1(a(n), ", "));