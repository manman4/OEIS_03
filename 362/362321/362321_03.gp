M=19;

a(n) = n! * polcoef(exp(x + n*x^4 + x*O(x^n)), n);
for(n=0, M, print1(a(n), ", ")); 
