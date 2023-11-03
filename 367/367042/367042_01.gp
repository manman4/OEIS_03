a(n) = sum(k=0, n\3, binomial(n-3*k+1,k) * binomial(2*(n-3*k),n-3*k)/(n-3*k+1) );
for(n=0, 27, print1(a(n), ", "))
