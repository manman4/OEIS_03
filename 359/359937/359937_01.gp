M=10000;

a(n) = sumdiv(n, d, issquare(d-1)*d);
for(n=1, M, write("/Users/xxx/Desktop/b357937_1.txt", n, " ", a(n)))