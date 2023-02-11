M=17;

a(n) = sum(k=0, n, (3*k)^(n-k)*(3*k+2)^(k-1)*binomial(n, k))/2^(n-1);
for(n=0, M, print1(a(n), ", ")) 
