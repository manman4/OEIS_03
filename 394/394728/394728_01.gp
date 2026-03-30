\\ Expansion of e.g.f. exp( sqrt(1-x^2) - 1 + x*arcsin(x) )
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp(sqrt(1-x^2)-1+x*asin(x))))

a(n) = my(x='x+O('x^(2*n+1))); (2*n)!*polcoef(exp(sqrt(1-x^2)-1+x*asin(x)), 2*n);
for(n=0, 16, print1(a(n), ", "));