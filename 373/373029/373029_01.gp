T(n, k) = my(x='x+x*O('x^(n+10))); polcoef(sum(i=0, n, x^(k*i) * prod(j=1, k*i-1, 1+x^j ) ), n); 

for(n=0, 13, for(k=0, n, print1(T(n, k), ", ")));
for(n=0, 25, print1(T(2*n, n), ", "));