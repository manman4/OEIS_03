my(A=1, n=23); for(i=1, n, A=(1-x+x*O(x^n))^(-exp(x+x*O(x^n))/A)); Vec(serlaplace(A))