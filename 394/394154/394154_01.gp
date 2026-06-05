S(n,k) = if(k==0, 1, (n/k) * sum(j=1, k, 2^j * binomial(k,j) * binomial(n+k-1,j-1)));
a(n) = (1/n) * sum(j=0, (n-1)\2, (-1)^j * binomial(n-1-j,j) * S(n,n-1-j));
for(n=1, 20, print1(a(n),", "));

