M=24;

a(n) = sum(k=0, n\3, (-1)^k*binomial(2*n-3*k, n)); 
for(n=0, M, print1(a(n),", "))

