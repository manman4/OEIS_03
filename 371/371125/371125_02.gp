a(n, m) = my(x='x+O('x^(6*n+1))); polcoef(1/(1-x^m-x^6), 6*n); 
for(n=0, 34, print1(a(n, 1),", "))      