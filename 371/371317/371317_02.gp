seq(n) = my(A=x); for(i=1, n, A=(exp(x +x*O(x^n)) - 1)/(1-A)^3 ); Vec(serlaplace(A));         
concat(0, seq(20))   