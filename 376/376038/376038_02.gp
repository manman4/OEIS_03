\\ E.g.f. satisfies A(x) = (-log(1 - x / (1 - A(x))^3)) * (1 - A(x))^2.
seq(n) = my(A=x); for(i=1, n, A=-log(1 - x/(1-A)^3 +x*O(x^n)) * (1 - A)^2 ); Vec(serlaplace(A));        
concat(0, seq(20))   