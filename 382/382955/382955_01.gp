M=139;

a(n, k) = my(x='x+O('x^(n+20)), y='y+O('y^(k+20))); polcoef(polcoef(prod(j=1, max(n, k)+10, 1+x^prime(j)+y^prime(j)), n), k); 
cnt=0;
for(n=0, M, for(k=0, n, write("/Users/xxx/Desktop/b382955_1.txt", cnt, " ", a(k, n-k)); cnt++));
