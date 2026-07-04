a(n) = if(n==0, 1, sum(k=1, n, k*binomial(2*k, k)*binomial(2*n-k-1, n-1))/n);

N=1000;
for(n=0, N, write("b350645_1.txt", n, " ", a(n)));


