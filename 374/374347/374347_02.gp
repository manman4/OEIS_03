\\ with A(0)=1　なし
a(n, k) = my(A=x^2); for(i=1, n, A = subst(A, x, x^k)^(1/k)*exp(k*x +x*O(x^n)) ); polcoeff(A, n);         
for(n=0, 35, print1(a(n, 3), ", "));    

a(n, k) = my(A=1); for(i=1, n, A = subst(A, x, x^k)^(1/k)*exp(k*x +x*O(x^n)) ); polcoeff(A, n);         
for(n=0, 35, print1(a(n, 3), ", "));    
for(n=0, 21, print1(n! * a(n, 3), ", "));    