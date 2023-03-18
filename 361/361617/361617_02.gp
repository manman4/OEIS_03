M=16;

a(n) = n! * polcoef(exp( x/(1-x +x*O(x^n))^n ) / (1-x +x*O(x^n))^n, n);
for(n=0, M, print1(a(n), ", "));
