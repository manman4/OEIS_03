a(n) = sum(k=0, n, binomial(4*k+1,k) * binomial(k,n-k)/(4*k+1) );
for(n=0, 21, print1(a(n), ", "))

b(n) = sum(k=0, n, binomial(k,n-k) * binomial(4*k, k)/(3*k + 1) );
for(n=0, 21, print1(a(n)-b(n), ", "))

c(n) = sum(k=0, n\2, binomial(3*n-2*k+1, k)*binomial(4*n-4*k, n-2*k)/(3*n-2*k+1));
for(n=0, 21, print1(a(n)-c(n), ", "))