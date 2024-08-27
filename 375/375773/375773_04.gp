\\ G.f.: Sum_{k>=0} (5*k)! * x^(5*k)/(k! * Product_{j=1..5*k} (1 - j * x)).
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, (5*k)!*x^(5*k)/(k!*prod(j=1, 5*k, 1-j*x))))

