a(n) = if(n==0, 1, n! * polcoeff(exp(1 - (1+x)^n + x*O(x^n)), n));   
for(n=0, 17, print1(a(n), ", ")); 