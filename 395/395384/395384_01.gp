\\ a(n) = (2*n)! * [x^(2*n)] 1 / cos(x)^6.
a(n) = my(x='x+O('x^(2*n+1))); (2*n)!*polcoef(1/cos(x)^6, 2*n);
for(n=0, 17, print1(a(n),", "));    

