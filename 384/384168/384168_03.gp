M=17;

\\ a(n) = n! * [x^n] ( -log(1 - 3*x)/(3 * (1 - 3*x)^(n/3+1)) ).
a(n) = my(x='x+O('x^(n+1))); n! * polcoef( (-log(1 - 3*x))/(3 * (1 - 3*x)^(n/3+1)), n);
for(n=1, M, print1(a(n),", "));
