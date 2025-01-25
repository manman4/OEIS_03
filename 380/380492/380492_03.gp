M=15;

\\ a(n) = n! * [x^n] exp(x/(1 - x))/(1 - x)^(n-1).
a(n) = my(x='x+O('x^(n+1))); n! * polcoef(exp(x/(1 - x))/(1 - x)^(n-1), n);
for(n=0, M, print1(a(n),", "))                 

