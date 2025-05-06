\\ a(n) = (2*n)! * [x^(2*n)] 1/f(x)^(4*n-3) * log(f(x))^n / n!, where f(x) = 1/(1 - 4*x)^(1/4).
a(n) = my(x='x+O('x^(n+10)), f(x) = 1/(1 - 4*x)^(1/4)); (2*n)! * polcoef( 1/f(x)^(4*n-3) * log(f(x))^n / n!, 2*n);
for(n=0, 20, print1(a(n), ", ")); 

