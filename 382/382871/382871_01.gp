M=74;

a(n) = my(x='x+O('x^(n+1)), y='y+O('y^(n+1))); if(n==0, 1, polcoef(polcoef(prod(k=1, n, 1+x^prime(k)+y^prime(k)), n), n)/2);
for(n=0, M, print1(a(n), ", "));

for(n=0, 100, if(a(n)==0, print1(n, ", ")));
