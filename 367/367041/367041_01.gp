a(n) = sum(k=0, n\2, binomial(3*(n-2*k)+1,k) * binomial(4*(n-2*k),n-2*k)/(3*(n-2*k)+1) );
for(n=0, 21, print1(a(n), ", "))
