a(n, m) = my(x='x+O('x^(7*n+1))); polcoef(1/(1-x^6-x^7), 7*n-m); 
for(n=1, 34, print1(a(n, 3),", "))      