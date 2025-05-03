my(N=20, x='x+O('x^N)); Vec(1/prod(k=0, 4, 1-(5*k+1)*x)^(1/5))

M=1000;
\\ Expansion of 1/( Product_{k=0..4} (1 - (5*k+1) * x) )^(1/5).
v(n) = my(x='x+O('x^(n+10))); 1/prod(k=0, 4, 1-(5*k+1)*x)^(1/5);
v = v(M);
for(n=0, M, if(denominator(polcoef(v, n)) != 1, print1(n, ", ")));

