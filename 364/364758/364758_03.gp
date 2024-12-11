\\ G.f. A(x) satisfies A(x)^3 = 1 + x*A(x) + x*A(x)^5 + x*A(x)^6.
seq(n) = my(A=1); for(i=1, n, A=(1 + x*A + x*A^5 + x*A^6 + x*O(x^n))^(1/3) ); Vec(A);       
seq(30)   
