M=17;

a(n) = local(A=1); for(i=1, n, A=exp(x* ( exp(x+x*O(x^n))*A )^2 )); n!*polcoeff(A, n); 
for(n=0, M, print1(a(n), ", ")) 

