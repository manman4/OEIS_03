M=20;

\\ a(n) = n! * [x^n] (1 + 5*x)^(n/5).
a(n) = my(x='x+O('x^(n+1))); n! * polcoef( (1 + 5*x)^(n/5), n);
for(n=0, M, print1(a(n),", "));
