a(n) = sum(k=0, n\2, (-1)^k * binomial(3*(n-2*k)+1,k) * binomial(4*(n-2*k),n-2*k)/(3*(n-2*k)+1) );
for(n=0, 22, print1(a(n), ", "))
