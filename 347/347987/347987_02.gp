M=17;

\\ a(n) = [x^(2*n)] ((2*n)!/n!) * (log(1 + x))^n/(1 - x)
a(n) = x='x+O('x^(2*n+1)); (2*n)!/n! * polcoef(log(1+x)^n/(1-x), 2*n);
for(n=0, M, print1(a(n), ", "));