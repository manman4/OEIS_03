a(n) = (1/(n+1)) * sum(k=0, n\5, (-1)^k * binomial(n+k,k) * binomial(2*n-4*k,n-5*k) );
for(n=0, 28, print1(a(n),", "))   

print1(a(32))
