M=25;

a(n) = sum(k=0, n-2, k!*binomial(n, k+1)); 
b(n) = if(n==0, 1, sum(k=1, n, a(k)*binomial(n, k)*b(n-k)));
for(n=0, M, print1(b(n), ", "));