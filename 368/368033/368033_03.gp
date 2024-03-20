seq(n) = my(A=x); for(i=1, n, A=log(1 + x/(1-A)^2 +x*O(x^n)) ); Vec(serlaplace(A));         
concat(0, seq(20))   