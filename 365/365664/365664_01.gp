M=10000;
v(M) = my(q='q+O('q^(M+100))); (1/9) * sum(k=4, M, (-1)^k * (2*k+1) * binomial(k+4,8) * q^(k*(k+1)/2)) / sum(k=0, M, (-1)^k * (2*k+1) * q^(k*(k+1)/2));
v=v(M);
for(n=10, M, write("/Users/xxx/Desktop/b365664_1.txt", n, " ", polcoef(v, n)))

