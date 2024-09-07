seq(n) = my(A=x); for(i=1, n, A=(exp(x/(1-A)^3 +x*O(x^n)) - 1)/(1-A) ); Vec(serlaplace(A));         
concat(0, seq(20))   