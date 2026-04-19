\\ a(n) = (2*n)! * [x^(2*n)] cos(2*x) * cos(3*x) * cos(4*x) / cos(24*x).
a(n) = my(x = 'x + O('x^(2*n+1))); (2*n)!*polcoef(cos(2*x) * cos(3*x) * cos(4*x) / cos(24*x), 2*n);
for(n=0, 15, print1(a(n),", "));   
\\ for(n=0, 15, print1(round(a(n)),", "));   

