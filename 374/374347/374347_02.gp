\\ with A(0)=1　なし
a(n, k) = my(A=x^2); for(i=1, n, A = subst(A, x, x^k)^(1/k)*exp(k*x +x*O(x^n)) ); polcoef(A, n);         
for(n=0, 35, print1(a(n, 3), ", "));    

a(n, k) = my(A=1); for(i=1, n, A = subst(A, x, x^k)^(1/k)*exp(k*x +x*O(x^n)) ); polcoef(A, n);         
for(n=0, 35, print1(a(n, 3), ", "));    
for(n=0, 35, print1(n! * a(n, 3), ", "));    

\\ A218003の利用
b(n) = if(n<=0, n==0, 3*sum(k=0, round(log(n)/log(3)), prod(j=1, 3^k-1, n-j) * b(n-3^k)));
for(n=0, 35, m=n! * a(n, 3); if(m == b(n), print1(m,", ")));