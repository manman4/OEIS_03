M=100;
a(n)= L=n^2+1; x='x+O('x^L); polcoef(sum(k=n, n^2, x^k/prod(j=1, k, 1-x^j)), n^2);

for(n=0, M, write("/Users/xxx/Desktop/b347585.txt", n, " ", a(n)))