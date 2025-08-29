my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(6*x) * BesselI(0, 2*sqrt(10)*x).
my(N=30, x='x+O('x^N)); Vec(apply(round, serlaplace(exp(6*x) * my_besseli(0, 2*sqrt(10)*x) )))
my(N=30, x='x+O('x^N)); Vec(apply(round, serlaplace(exp(6*x) * my_besseli(0, 2*sqrt(10)*x) - exp(6*x) * sum(k=0, N, (2*sqrt(10)*x/2)^(2*k)/(k! * k!) ))))
my(N=30, x='x+O('x^N)); serlaplace(exp(6*x) * my_besseli(0, 2*sqrt(10)*x) )