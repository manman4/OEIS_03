a(n,k)={x='x+O('x^(n+50)); polcoef(serlaplace(exp(k*(exp(x) - 1) - x)),n)};
for(n=0, 20, print1(a(n,n), ", "))
