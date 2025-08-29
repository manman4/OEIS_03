my_besseli(n, x) = besseli(n, x)*(x/2)^n/n!;

\\ E.g.f.: exp(2*x) * BesselI(1, 2*sqrt(2)*x) / sqrt(2), with offset 1.
my(N=30, x='x+O('x^N)); Vec(apply(round, serlaplace(exp(2*x) * my_besseli(1, 2*sqrt(2)*x) / sqrt(2))))
my(N=30, x='x+O('x^N)); Vec(apply(round, serlaplace(exp(2*x) * my_besseli(1, 2*sqrt(2)*x) / sqrt(2) - exp(2*x) * sum(k=0, N, (2*sqrt(2)*x/2)^(2*k+1)/(k! * (k+1)!) / sqrt(2)))))
my(N=30, x='x+O('x^N)); serlaplace(exp(2*x) * my_besseli(1, 2*sqrt(2)*x) / sqrt(2))