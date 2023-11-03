a(n) = sum(k=0, n\3, (-1)^k * binomial(3*(n-3*k)+1,k) * binomial(4*(n-3*k),n-3*k)/(3*(n-3*k)+1) );
for(n=0, 21, print1(a(n), ", "))
