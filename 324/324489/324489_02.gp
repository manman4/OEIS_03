f(x) = ((1-3*x+x^2)*(1+3*x+x^2))^3*(1-x^2)^10/((1-4*x-x^2)*(1-x-x^2)^6*(1+x-x^2)^9);
my(N=40, x='x+O('x^N)); Vec(sum(k=1, N, moebius(k)*log(f(x^k))/k))