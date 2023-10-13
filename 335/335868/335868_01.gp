a(n, k)={x='x+O('x^(n+50)); polcoef(serlaplace(exp(k*(1-exp(x))-x)), n)};
M=200;
for(n=0, M, i=a(n, n); if((i<0)+#digits(i)>1000, break); write("/Users/xxx/Desktop/b335868_gp_test.txt", n, " ", i))