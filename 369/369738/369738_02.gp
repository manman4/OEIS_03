T(n,k) = if(k==0, 0^n, n! * polcoeff(exp(1 - (1+x)^k + x*O(x^n)), n));   
for(n=0, 10, for(k=0, n, print1(T(k,n-k), ", "))); 