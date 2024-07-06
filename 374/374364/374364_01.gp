M=200;

my(x='x+O('x^(M+10))); v=serlaplace(exp(x-sum(k=1, ceil(log(M+1)/log(2)), x^2^k/2^k)));
for(n=0, M, write("/Users/xxx/Desktop/b374364.txt", n, " ", polcoef(v, n)))
