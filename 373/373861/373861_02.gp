\\ E.g.f.: Sum_{k>=0} (-log(1 - k^2*x))^k / k!.

my(N=20, x='x+O('x^N)); Vec(serlaplace(sum(k=0, N, (-log(1-k^2*x))^k/k! )))