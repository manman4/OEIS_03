a000364(n) = if(n<0, 0, (2*n)! * polcoeff( 1 / cos(x + O(x^(2*n + 1))), 2*n));
for(n=0, 17, print1(a000364(n), ", "));

\\ a(n) = (2*n)! [x^(2*n)] cosh(x)^(-3).
a(n) = my(x='x+O('x^(2*n+1))); (2*n)!*polcoef(1/cosh(x)^3, 2*n);
for(n=0, 17, print1(a(n),", "));    

