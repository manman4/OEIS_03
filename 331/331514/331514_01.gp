S(n,k) = (1/2)*sum(j=1, n+1, (k-2)^(n+1-j)*j*binomial(n+1, j)*binomial(n+1+j, j));
for(n=0, 10, for(k=0, n, print1(S(k,n-k),", ")));

T(n,k) = if(k==0, S(n,k), sum(j=0, n, (k/2)^j * (-(k-2)^2/(2*k))^(n-j) * (2*j+1) * binomial(2*j,j) * binomial(j,n-j))   );

for(n=0, 10, for(k=0, n, print1(T(k,n-k)-S(k,n-k),", ")))        
