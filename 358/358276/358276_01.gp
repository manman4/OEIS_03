a(n) = local(A=x); for(i=1, n, A=x - sum(k=2, n, (-1)^k*k*subst(A, x, x^k) +x*O(x^n) )); polcoeff(A, n); 
for(n=1, 68, print1(a(n),", "))    
