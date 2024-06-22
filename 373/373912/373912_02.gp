a(n, m) = my(x='x+O('x^(7*n+1))); polcoef(1/(1-x^m-x^7), 7*n); 
for(n=0, 34, print1(a(n, 6),", "))      