default(parisize, 12000000000)

M=1000;
a(n, k) = my(x='x+O('x^(n+10))); sum(i=1, n+10, i!*x^(i*(i+2*k-1)/2)/prod(j=1, i-1, 1-x^j));
v=a(M, 3);
for(n=0, M, write("b339164_1.txt", n, " ", polcoef(v, n)))