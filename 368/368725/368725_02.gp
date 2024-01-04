\\ E.g.f.: Sum_{k>=0} (k * x)^k / (k! * (1 + k * x)).
my(N=30, x='x+O('x^N)); Vec(serlaplace( sum(k=0, N, (k * x)^k / (k! * (1 + k * x))) ))
