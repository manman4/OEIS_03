a(n) = my(A=1); for(i=1, n, A = 1 + x*A^2 +x*O(x^n) ); Vec(x * (A/(1+x*A))^2); 
a(30)     


a(n) = my(A=1); for(i=1, n, A = 1 + x*A^2 +x*O(x^n) ); Vec(x^2 * (A/(1+x*A))^3); 
a(30)     




a(n) = my(A=1); for(i=1, n, A = 1 + x*A^3 +x*O(x^n) ); Vec(x * (A/(1+x*A))^2); 
a(30)     


a(n) = my(A=1); for(i=1, n, A = 1 + x*A^3 +x*O(x^n) ); Vec(x^2 * (A/(1+x*A))^3); 
a(30)     