\\ exp((x + x^2/2 + x^3/6)/(1-x))
my(N=30, x='x+O('x^N)); Vec(serlaplace(exp((x+x^2/2+x^3/6)/(1-x))))

