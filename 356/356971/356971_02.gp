my(A=1, n=20); for(i=1, n, A=(1-x*A+x*O(x^n))^(-x^3*A+x*O(x^n))); Vec(serlaplace(A))