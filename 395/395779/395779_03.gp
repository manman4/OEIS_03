\\ a(n) = Sum_{i,j,k >= 0 and i+j+k=n} binomial(3*i+1,i) * binomial(3*j+1,j) * binomial(3*k+1,k).
a(n) = sum(i=0, n, sum(j=0, n-i, binomial(3*i+1,i) * binomial(3*j+1,j) * binomial(3*(n-i-j)+1,n-i-j)));
for(n=0, 10, print1(a(n), ", "));