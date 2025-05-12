\\ a(n) = (3*n)!/n! * [x^(3*n)] sinh(x)^n.
a(n) = my(x='x+O('x^(3*n+1))); (3*n)!/n! * polcoef(sinh(x)^n, 3*n);
for(n=0, 20, print1(a(n),", "));

