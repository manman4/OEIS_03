\\ a(n) = Sum_{i+j+k=n, i,j,k >= 0} binomial(3*i,i) * binomial(3*j,j) * binomial(3*k,k).
a(n) = sum(i=0, n, sum(j=0, n-i, binomial(3*i,i) * binomial(3*j,j) * binomial(3*(n-i-j),n-i-j)));
for(n=0, 21, print1(a(n), ", "));