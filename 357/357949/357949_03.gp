\\ G.f.: Sum_{k>=0} k! * x^k/(1-x^4)^(k+1). 
my(N=30, x='x+O('x^N)); Vec(sum(k=0, N, k! * x^k/(1-x^4)^(k+1) ))
