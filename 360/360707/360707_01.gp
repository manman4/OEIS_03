a(n) = sum(k=0, n, binomial(k,n-3*k) * k^(n-3*k));
M=1000;
for(n=0, M, write("/Users/xxx/Desktop/b360707_1.txt", n, " ", a(n)))