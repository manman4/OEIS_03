\\ a(n) = n! * [x^n] exp(-n*x) / (1-x)^(n+1).
a(n) = if(n==0, 1, my(x='x+O('x^(n+1))); n! * polcoef( exp(-n*x) / (1-x)^(n+1), n));
for(n=0, 20, print1(a(n), ", "));

