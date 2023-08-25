a(n) = sum(k=0, n, binomial(4*k+1,k) * binomial(k,n-k)/(4*k+1) );
for(n=0, 21, print1(a(n), ", "))

b(n) = sum(k=0, n, binomial(k,n-k) * binomial(4*k, k)/(3*k + 1) );
for(n=0, 21, print1(a(n)-b(n), ", "))