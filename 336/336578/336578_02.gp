a(n) = sum(k=0, n, 2^(n-k)*binomial(n, k)*binomial(n^2+k+1, n)/(n^2+k+1)); 
for(n=0, 19, print1(a(n),", "))          

