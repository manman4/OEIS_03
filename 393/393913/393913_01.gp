\\ G.f. A(x) satisfies A(x) = (1-x+2*x^2)/(1-x)^3 + x*A(x)^2.
my(N=30, x='x+O('x^N)); Vec((1-sqrt(1-4*x*((1-x+2*x^2)/(1-x)^3)))/(2*x))                  