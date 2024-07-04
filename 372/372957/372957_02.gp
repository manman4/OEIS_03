a(n, k) = my(A=x); for(i=1, n, A = ( subst(A, x, x^k)/(1 - k*x +x*O(x^n))^k )^(1/k)); polcoeff(A, n); 
for(n=1, 35, print1(a(n, 2), ", "));

