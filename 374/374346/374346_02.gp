\\ with A(0)=1　なし
a(n, k) = my(A=x^2); for(i=1, n, A = subst(A, x, x^k)^(1/k)*exp(k*x +x*O(x^n)) ); polcoeff(A, n);         
for(n=0, 35, print1(a(n, 2), ", "));    

a(n, k) = my(A=1); for(i=1, n, A = subst(A, x, x^k)^(1/k)*exp(k*x +x*O(x^n)) ); polcoeff(A, n);         
for(n=0, 35, print1(a(n, 2), ", "));    
for(n=0, 35, print1(n! * a(n, 2), ", "));   

b(n) = if(n<=0, n==0, 2*sum(k=0, round(log(n)/log(2)), prod(j=1, 2^k-1, n-j) * b(n-2^k)));
for(n=0, 35, m=n! * a(n, 2); if(m == b(n), print1(m,", ")));